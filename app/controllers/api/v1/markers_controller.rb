class Api::V1::MarkersController < ApplicationController
  respond_to :json

  def show
    respond_with Marker.find_by id: params[:id]
  end
end
