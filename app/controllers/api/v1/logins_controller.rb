class Api::V1::LoginsController < ApplicationController
  respond_to :json
  skip_before_action :verify_authenticity_token

  def create
    email = params["email"]
    password = params["password"]

    user = User.find_by_email(email)
    if user && user.valid_password?(password)
      render :json => {error: false}
    else
      render :json => {error: true}
    end
  end
end
