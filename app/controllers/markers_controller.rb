class MarkersController < ApplicationController
  before_action :authenticate_user!

  def index
    @marker_users = MarkerUser.all
  end

  def new
    @marker = Marker.new
  end

  def create
    @marker = Marker.new(marker_params)

    if @marker.save
      current_user.marker_users.create(marker_id: @marker.id, content: params["content"])
      GentexdataWorker.perform_async(@marker.id)
      redirect_to markers_path, notice: "The marker #{@marker.name} has been created."
    else
      render "new"
    end
  end

  def destroy
    @marker_user = MarkerUser.find params[:id]
    @marker = Marker.find_by id: @marker_user.marker_id
    @marker_user.destroy
    @marker.destroy
    redirect_to markers_path
  end

  def show
    @marker = Marker.find_by id: params[:id]
  end

  private
  def marker_params
    params.require(:marker).permit(:name, :image)
  end
end
