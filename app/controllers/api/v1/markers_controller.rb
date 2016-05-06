class Api::V1::MarkersController < ApplicationController
  respond_to :json
  skip_before_action :verify_authenticity_token
  before_action :authenticate_with_token!, only: [:create]

  def index
    @marker_users = MarkerUser.all
  end

  def show
    @marker = Marker.find_by id: params[:id]
  end


  def destroy
    @marker = Marker.find_by id: params[:id]
    @marker_user = MarkerUser.find_by marker_id: params[:id]

    @marker_user.destroy
    @marker.destroy
    render json: { success: "Marker deleted" }, status: 200
  end

  def create
    @current_user_api

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
    @marker.save
    @marker_user = @current_user_api.marker_users.create(marker_id: @marker.id, content: temp["content"])
    if @marker_user
      GentexdataWorker.perform_async(@marker.id)
      @marker_user
    else
      render json: { errors: "Create marker fail" }, status: 200
    end
  end

end
