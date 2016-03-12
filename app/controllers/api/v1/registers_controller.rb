class Api::V1::RegistersController < ApplicationController
  respond_to :json
  skip_before_action :verify_authenticity_token

  def create
    email = params["email"]
    password = params["password"]
    password_confirmation = params["password_confirmation"]
    reg_token = params["reg_token"]
    name = params["name"]
    @user = User.create(name: name, email: email, password: password, password_confirmation: password_confirmation, reg_token: reg_token)
    if @user.errors.any?
      render :json => {error: true}
    else
      render :json => {error: false}
    end
  end
end
