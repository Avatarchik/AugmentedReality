#ifdef _WIN32
#include <windows.h>
#  define truncf(x) floorf(x) // These are the same for positive numbers.
#endif
#include <stdio.h>
#include <string.h>
#include <AR/ar.h>
#include <AR2/config.h>
#include <AR2/imageFormat.h>
#include <AR2/imageSet.h>
#include <AR2/featureSet.h>
#include <AR2/util.h>
#include <KPM/kpm.h>
#ifdef _WIN32
#  define MAXPATHLEN MAX_PATH
#else
#  include <sys/param.h> // MAXPATHLEN
#endif
#if defined(__APPLE__) || defined(__linux__)
#  define HAVE_DAEMON_FUNC 1
#  include <unistd.h>
#endif
#include <time.h> // time(), localtime(), strftime()

#define          KPM_SURF_FEATURE_DENSITY_L0    70
#define          KPM_SURF_FEATURE_DENSITY_L1   100
#define          KPM_SURF_FEATURE_DENSITY_L2   150
#define          KPM_SURF_FEATURE_DENSITY_L3   200

#define          TRACKING_EXTRACTION_LEVEL_DEFAULT 2
#define          INITIALIZATION_EXTRACTION_LEVEL_DEFAULT 1
#define KPM_MINIMUM_IMAGE_SIZE 28 // Filter size for 1 octaves plus 1.
//#define KPM_MINIMUM_IMAGE_SIZE 196 // Filter size for 4 octaves plus 1.

#ifndef MIN
#  define MIN(x,y) (x < y ? x : y)
#endif

enum {
    E_NO_ERROR = 0,
    E_BAD_PARAMETER = 64,
    E_INPUT_DATA_ERROR = 65,
    E_USER_INPUT_CANCELLED = 66,
    E_BACKGROUND_OPERATION_UNSUPPORTED = 69,
    E_DATA_PROCESSING_ERROR = 70,
    E_UNABLE_TO_DETACH_FROM_CONTROLLING_TERMINAL = 71,
    E_GENERIC_ERROR = 255
};

static int                  genfset = 1;
static int                  genfset3 = 1;

static char                 filename[MAXPATHLEN] = "";
static AR2JpegImageT       *jpegImage;
//static ARUint8             *image;
static int                  xsize, ysize;
static int                  nc;
static float                dpi = -1.0f;

static float                dpiMin = -1.0f;
static float                dpiMax = -1.0f;
static float               *dpi_list;
static int                  dpi_num = 0;

static float                sd_thresh  = -1.0f;
static float                min_thresh = -1.0f;
static float                max_thresh = -1.0f;
static int                  featureDensity = -1;
static int                  occ_size = -1;
static int                  tracking_extraction_level = -1; // Allows specification from command-line.
static int                  initialization_extraction_level = -1;

static int                  background = 0;
static char                 logfile[MAXPATHLEN] = "";
static char                 exitcodefile[MAXPATHLEN] = "";
static char                 exitcode = 255;
#define EXIT(c) {exitcode=c;exit(c);}


static void  usage( char *com );
static int   readImageFromFile(const char *filename, ARUint8 **image_p, int *xsize_p, int *ysize_p, int *nc_p, float *dpi_p);
static int   setDPI( void );
static void  write_exitcode(void);

int genTexData( char *file)
{
    AR2JpegImageT       *jpegImage = NULL;
    ARUint8             *image = NULL;
    AR2ImageSetT        *imageSet = NULL;
    AR2FeatureMapT      *featureMap = NULL;
    AR2FeatureSetT      *featureSet = NULL;
    KpmRefDataSet       *refDataSet = NULL;
    float                scale1, scale2;
    int                  procMode;
    char                 buf[1024];
    int                  num;
    int                  i, j;
    char                *sep = NULL;
	time_t				 clock;
    int                  maxFeatureNum;
    int                  err;


    strncpy(filename, file, sizeof(filename) - 1);
    // Do some checks on the input.
    if (filename[0] == '\0') {
        ARLOGe("Error: no input file specified. Exiting.\n");

    }
    sep = strrchr(filename, '.');
    if (!sep || (strcmp(sep, ".jpeg") && strcmp(sep, ".jpg") && strcmp(sep, ".jpe") && strcmp(sep, ".JPEG") && strcmp(sep, ".JPE") && strcmp(sep, ".JPG"))) {
        ARLOGe("Error: input file must be a JPEG image (with suffix .jpeg/.jpg/.jpe). Exiting.\n");
    }

    if (logfile[0]) {
        if (!freopen(logfile, "a", stdout) ||
            !freopen(logfile, "a", stderr)) ARLOGe("Unable to redirect stdout or stderr to logfile.\n");
    }
    if (exitcodefile[0]) {
        atexit(write_exitcode);
    }

    // Print the start date and time.
    clock = time(NULL);
    if (clock != (time_t)-1) {
        struct tm *timeptr = localtime(&clock);
        if (timeptr) {
            char stime[26+8] = "";
            if (strftime(stime, sizeof(stime), "%Y-%m-%d %H:%M:%S %z", timeptr)) // e.g. "1999-12-31 23:59:59 NZDT".
                ARLOGi("--\nGenerator started at %s\n", stime);
        }
    }

    tracking_extraction_level = 2;

    if( sd_thresh  == -1.0f ) sd_thresh  = AR2_DEFAULT_SD_THRESH_L2;
    if( min_thresh == -1.0f ) min_thresh = AR2_DEFAULT_MIN_SIM_THRESH_L2;
    if( max_thresh == -1.0f ) max_thresh = AR2_DEFAULT_MAX_SIM_THRESH_L2;
    if( occ_size   == -1    ) occ_size   = AR2_DEFAULT_OCCUPANCY_SIZE*2/3;

    ARLOGi("MAX_THRESH  = %f\n", max_thresh);
    ARLOGi("MIN_THRESH  = %f\n", min_thresh);
    ARLOGi("SD_THRESH   = %f\n", sd_thresh);

    initialization_extraction_level = 3;
    if( featureDensity  == -1 ) featureDensity  = KPM_SURF_FEATURE_DENSITY_L3;

    ARLOGi("SURF_FEATURE = %d\n", featureDensity);

    if ((err = readImageFromFile(filename, &image, &xsize, &ysize, &nc, &dpi)) != 0) {
        ARLOGe("Error reading image from file '%s'.\n", filename);
        EXIT(err);
    }

    setDPI();

    ARLOGi("Generating ImageSet...\n");
    ARLOGi("   (Source image xsize=%d, ysize=%d, channels=%d, dpi=%.1f).\n", xsize, ysize, nc, dpi);
    imageSet = ar2GenImageSet( image, xsize, ysize, nc, dpi, dpi_list, dpi_num );
    ar2FreeJpegImage(&jpegImage);
    if( imageSet == NULL ) {
        ARLOGe("ImageSet generation error!!\n");
        EXIT(E_DATA_PROCESSING_ERROR);
    }
    ARLOGi("  Done.\n");
    ar2UtilRemoveExt( filename );
    ARLOGi("Saving to %s.iset...\n", filename);
    if( ar2WriteImageSet( filename, imageSet ) < 0 ) {
        ARLOGe("Save error: %s.iset\n", filename );
        EXIT(E_DATA_PROCESSING_ERROR);
    }
    ARLOGi("  Done.\n");

    if (genfset) {
        arMalloc( featureSet, AR2FeatureSetT, 1 );                      // A featureSet with a single image,
        arMalloc( featureSet->list, AR2FeaturePointsT, imageSet->num ); // and with 'num' scale levels of this image.
        featureSet->num = imageSet->num;

        ARLOGi("Generating FeatureList...\n");
        for( i = 0; i < imageSet->num; i++ ) {
            ARLOGi("Start for %f dpi image.\n", imageSet->scale[i]->dpi);

            featureMap = ar2GenFeatureMap( imageSet->scale[i],
                                          AR2_DEFAULT_TS1*AR2_TEMP_SCALE, AR2_DEFAULT_TS2*AR2_TEMP_SCALE,
                                          AR2_DEFAULT_GEN_FEATURE_MAP_SEARCH_SIZE1, AR2_DEFAULT_GEN_FEATURE_MAP_SEARCH_SIZE2,
                                          AR2_DEFAULT_MAX_SIM_THRESH2, AR2_DEFAULT_SD_THRESH2 );
            if( featureMap == NULL ) {
                ARLOGe("Error!!\n");
                EXIT(E_DATA_PROCESSING_ERROR);
            }
            ARLOGi("  Done.\n");


            featureSet->list[i].coord = ar2SelectFeature2( imageSet->scale[i], featureMap,
                                                          AR2_DEFAULT_TS1*AR2_TEMP_SCALE, AR2_DEFAULT_TS2*AR2_TEMP_SCALE, AR2_DEFAULT_GEN_FEATURE_MAP_SEARCH_SIZE2,
                                                          occ_size,
                                                          max_thresh, min_thresh, sd_thresh, &num );
            if( featureSet->list[i].coord == NULL ) num = 0;
            featureSet->list[i].num   = num;
            featureSet->list[i].scale = i;

            scale1 = 0.0f;
            for( j = 0; j < imageSet->num; j++ ) {
                if( imageSet->scale[j]->dpi < imageSet->scale[i]->dpi ) {
                    if( imageSet->scale[j]->dpi > scale1 ) scale1 = imageSet->scale[j]->dpi;
                }
            }
            if( scale1 == 0.0f ) {
                featureSet->list[i].mindpi = imageSet->scale[i]->dpi * 0.5f;
            }
            else {
                /*
                 scale2 = imageSet->scale[i]->dpi;
                 scale = sqrtf( scale1 * scale2 );
                 featureSet->list[i].mindpi = scale2 / ((scale2/scale - 1.0f)*1.1f + 1.0f);
                 */
                featureSet->list[i].mindpi = scale1;
            }

            scale1 = 0.0f;
            for( j = 0; j < imageSet->num; j++ ) {
                if( imageSet->scale[j]->dpi > imageSet->scale[i]->dpi ) {
                    if( scale1 == 0.0f || imageSet->scale[j]->dpi < scale1 ) scale1 = imageSet->scale[j]->dpi;
                }
            }
            if( scale1 == 0.0f ) {
                featureSet->list[i].maxdpi = imageSet->scale[i]->dpi * 2.0f;
            }
            else {
                //scale2 = imageSet->scale[i]->dpi * 1.2f;
                scale2 = imageSet->scale[i]->dpi;
                /*
                 scale = sqrtf( scale1 * scale2 );
                 featureSet->list[i].maxdpi = scale2 * ((scale/scale2 - 1.0f)*1.1f + 1.0f);
                 */
                featureSet->list[i].maxdpi = scale2*0.8f + scale1*0.2f;
            }

            ar2FreeFeatureMap( featureMap );
        }
        ARLOGi("  Done.\n");

        ARLOGi("Saving FeatureSet...\n");
        if( ar2SaveFeatureSet( filename, "fset", featureSet ) < 0 ) {
            ARLOGe("Save error: %s.fset\n", filename );
            EXIT(E_DATA_PROCESSING_ERROR);
        }
        ARLOGi("  Done.\n");
        ar2FreeFeatureSet( &featureSet );
    }

    if (genfset3) {
        ARLOGi("Generating FeatureSet3...\n");
        refDataSet  = NULL;
        procMode    = KpmProcFullSize;
        for( i = 0; i < imageSet->num; i++ ) {
            //if( imageSet->scale[i]->dpi > 100.0f ) continue;

            maxFeatureNum = featureDensity * imageSet->scale[i]->xsize * imageSet->scale[i]->ysize / (480*360);
            ARLOGi("(%d, %d) %f[dpi]\n", imageSet->scale[i]->xsize, imageSet->scale[i]->ysize, imageSet->scale[i]->dpi);
            if( kpmAddRefDataSet (
#if AR2_CAPABLE_ADAPTIVE_TEMPLATE
                                  imageSet->scale[i]->imgBWBlur[1],
#else
                                  imageSet->scale[i]->imgBW,
#endif
                                  AR_PIXEL_FORMAT_MONO,
                                  imageSet->scale[i]->xsize,
                                  imageSet->scale[i]->ysize,
                                  imageSet->scale[i]->dpi,
                                  procMode, KpmCompNull, maxFeatureNum, 1, i, &refDataSet) < 0 ) { // Page number set to 1 by default.
                ARLOGe("Error at kpmAddRefDataSet.\n");
                EXIT(E_DATA_PROCESSING_ERROR);
            }
        }
        ARLOGi("  Done.\n");
        ARLOGi("Saving FeatureSet3...\n");
        if( kpmSaveRefDataSet(filename, "fset3", refDataSet) != 0 ) {
            ARLOGe("Save error: %s.fset2\n", filename );
            EXIT(E_DATA_PROCESSING_ERROR);
        }
        ARLOGi("  Done.\n");
        kpmDeleteRefDataSet( &refDataSet );
    }

    ar2FreeImageSet( &imageSet );

    // Print the start date and time.
    clock = time(NULL);
    if (clock != (time_t)-1) {
        struct tm *timeptr = localtime(&clock);
        if (timeptr) {
            char stime[26+8] = "";
            if (strftime(stime, sizeof(stime), "%Y-%m-%d %H:%M:%S %z", timeptr)) // e.g. "1999-12-31 23:59:59 NZDT".
                ARLOGi("Generator finished at %s\n--\n", stime);
        }
    }

    exitcode = E_NO_ERROR;
    return (exitcode);
}

// Reads dpiMinAllowable, xsize, ysize, dpi, background, dpiMin, dpiMax.
// Sets dpiMin, dpiMax, dpi_num, dpi_list.
static int setDPI( void )
{
    float       dpiWork, dpiMinAllowable;
    char		buf1[256];
    int			i;

    // Determine minimum allowable DPI, truncated to 3 decimal places.
    dpiMinAllowable = truncf(((float)KPM_MINIMUM_IMAGE_SIZE / (float)(MIN(xsize, ysize))) * dpi * 1000.0) / 1000.0f;

    if (background) {
        if (dpiMin == -1.0f) dpiMin = dpiMinAllowable;
        if (dpiMax == -1.0f) dpiMax = dpi;
    }
    dpiMin = 20;
    dpiMax = 120;


    // Decide how many levels we need.
    if (dpiMin == dpiMax) {
        dpi_num = 1;
    } else {
        dpiWork = dpiMin;
        for( i = 1;; i++ ) {
            dpiWork *= powf(2.0f, 1.0f/3.0f); // *= 1.25992104989487
            if( dpiWork >= dpiMax*0.95f ) {
                break;
            }
        }
        dpi_num = i + 1;
    }
    arMalloc(dpi_list, float, dpi_num);

    dpiWork = dpiMin;
    for( i = 0; i < dpi_num; i++ ) {
        ARLOGi("Image DPI (%d): %f\n", i+1, dpiWork);
        dpi_list[dpi_num - i - 1] = dpiWork; // Lowest value goes at tail of array, highest at head.
        dpiWork *= powf(2.0f, 1.0f/3.0f);
        if( dpiWork >= dpiMax*0.95f ) dpiWork = dpiMax;
    }

    return 0;
}


static int readImageFromFile(const char *filename, ARUint8 **image_p, int *xsize_p, int *ysize_p, int *nc_p, float *dpi_p)
{
    char *ext;
    char buf[256];
    char buf1[512], buf2[512];

    if (!filename || !image_p || !xsize_p || !ysize_p || !nc_p || !dpi_p) return (E_BAD_PARAMETER);

    ext = arUtilGetFileExtensionFromPath(filename, 1);
    if (!ext) {
        ARLOGe("Error: unable to determine extension of file '%s'. Exiting.\n", filename);
        EXIT(E_INPUT_DATA_ERROR);
    }
    if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0 || strcmp(ext, "jpe") == 0) {

        ARLOGi("Reading JPEG file...\n");
        ar2UtilDivideExt( filename, buf1, buf2 );
        jpegImage = ar2ReadJpegImage( buf1, buf2 );
        if( jpegImage == NULL ) {
            ARLOGe("Error: unable to read JPEG image from file '%s'. Exiting.\n", filename);
            EXIT(E_INPUT_DATA_ERROR);
        }
        ARLOGi("   Done.\n");

        *image_p = jpegImage->image;
        if (jpegImage->nc != 1 && jpegImage->nc != 3) {
            ARLOGe("Error: Input JPEG image is in neither RGB nor grayscale format. %d bytes/pixel %sformat is unsupported. Exiting.\n", jpegImage->nc, (jpegImage->nc == 4 ? "(possibly CMYK) " : ""));
            EXIT(E_INPUT_DATA_ERROR);
        }
        *nc_p    = jpegImage->nc;
        ARLOGi("JPEG image '%s' is %dx%d.\n", filename, jpegImage->xsize, jpegImage->ysize);
        if (jpegImage->xsize < KPM_MINIMUM_IMAGE_SIZE || jpegImage->ysize < KPM_MINIMUM_IMAGE_SIZE) {
            ARLOGe("Error: JPEG image width and height must be at least %d pixels. Exiting.\n", KPM_MINIMUM_IMAGE_SIZE);
            EXIT(E_INPUT_DATA_ERROR);
        }
        *xsize_p = jpegImage->xsize;
        *ysize_p = jpegImage->ysize;
        if (*dpi_p == -1.0) {
            if( jpegImage->dpi == 0.0f ) {
                jpegImage->dpi = 220;
            }
            *dpi_p   = jpegImage->dpi;
        }

    } else {
        ARLOGe("Error: file '%s' has extension '%s', which is not supported for reading. Exiting.\n", filename, ext);
        free(ext);
        EXIT(E_INPUT_DATA_ERROR);
    }
    free(ext);

    return 0;
}

static void write_exitcode(void)
{
    if (exitcodefile[0]) {
        FILE *fp = fopen(exitcodefile, "w");
        fprintf(fp, "%d\n", exitcode);
        fclose(fp);
    }
}

