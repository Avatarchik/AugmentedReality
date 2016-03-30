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
      current_user.marker_users.create(marker_id: @marker.id)
      GentexdataWorker.perform_async(@marker.id)
      redirect_to markers_path, notice: "The marker #{@marker.name} has been created."
    else
      render "new"
    end
  end

  def destroy
    @marker = MarkerUser.find params[:id]
    @marker.destroy
    redirect_to markers_path
  end

  def show
  end

  private
  def marker_params
    params.require(:marker).permit(:name, :image)
  end
end
