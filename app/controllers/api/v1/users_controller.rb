class Api::V1::UsersController < ApplicationController
  respond_to :json
  skip_before_action :verify_authenticity_token

  def show
    respond_with User.find_by id: params[:id]
  end

  def create
    email = params["email"]
    password = params["password"]
    password_confirmation = params["password_confirmation"]
    reg_token = params["reg_token"]
    name = params["name"]
    @user = User.create(name: name, email: email, password: password,
      password_confirmation: password_confirmation, reg_token: reg_token)
    if @user.errors.any?
      render json: { errors: @user.errors }, status: 422
    else
      render json: @user, status: 201, location: [:api, @user]
    end
  end
end
