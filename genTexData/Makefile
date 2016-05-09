AR2_HOME= ../..
AR2_INC_DIR= $(AR2_HOME)/include
AR2_LIB_DIR= $(AR2_HOME)/lib

BIN_DIR= ../../bin

CC= gcc
CXX= g++ -std=c++11
CFLAG= -O3 -fPIC -march=core2 -DHAVE_NFT=1 -I/usr/include/x86_64-linux-gnu -I$(AR2_INC_DIR)/linux-x86_64 -I$(AR2_INC_DIR)
LDFLAG= -L/usr/lib/x86_64-linux-gnu -L/usr/lib64 -L$(AR2_LIB_DIR)/linux-x86_64 -L$(AR2_LIB_DIR)
LIBS= -lKPM -lAR2 -lARvideo -lARgsub -lAR -lARICP -lAR -lARUtil -ljpeg -lglut -lGLU -lGL -lX11 -lm -lpthread -ljpeg -ldc1394 -lraw1394

OBJS =
HEADDERS =

all: $(BIN_DIR)/genTexData

$(BIN_DIR)/genTexData: genTexData.o $(OBJS)
	${CXX} -o $(BIN_DIR)/genTexData genTexData.o $(OBJS) $(LDFLAG) $(LIBS)

genTexData.o: genTexData.c $(HEADDERS)
	${CC} -c $(CFLAG) genTexData.c


clean:
	rm -f *.o
	rm -f $(BIN_DIR)/genTexData

allclean:
	rm -f *.o
	rm -f $(BIN_DIR)/genTexData
	rm -f Makefile

distclean:
	rm -f *.o
	rm -f Makefile
