class Api::V1::MarkersController < ApplicationController
  respond_to :json
  skip_before_action :verify_authenticity_token

  def index
    @markers = Marker.all
  end

  def show
    @marker = Marker.find_by id: params[:id]
  end

  def create
    temp = params
    params = {
      :marker => {
      :name => temp["name"],
      :image =>
      {
        :file => temp["base64"],
        :filename => (temp[:name] + ".jpg")
        }
      }
    }
    if params[:marker][:image][:file]
       image_params = params[:marker][:image]
       tempfile = Tempfile.new("fileupload")
       tempfile.binmode
       tempfile.write(Base64.decode64(image_params[:file]))
       uploaded_file = ActionDispatch::Http::UploadedFile.new(:tempfile => tempfile, :filename => image_params[:filename], :original_filename => image_params[:filename])
       params[:marker][:image] =  uploaded_file
    end

    @marker = Marker.new(params[:marker])

    respond_to do |format|
      if @marker.save
        GentexdataWorker.perform_async(@marker.id)
        format.html { redirect_to @marker, notice: 'Marker was successfully created.' }
        format.json { render json: @marker, status: :created, location: @marker }
      else
        format.html { render action: "new" }
        format.json { render json: @marker.errors, status: :unprocessable_entity }
      end
    end
  end

end
