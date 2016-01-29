class MarkersController < ApplicationController
  def index
    @markers = Marker.all
  end

  def new
    @marker = Marker.new
  end

  def create
    @marker = Marker.new(marker_params)

    if @marker.save
      redirect_to markers_path, notice: "The marker #{@marker.name} has been uploaded."
    else
      render "new"
    end
  end

  def destroy
  end

  def show
  end

  private
  def marker_params
    params.require(:marker).permit(:name, :image)
  end
end
