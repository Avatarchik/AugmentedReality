class Api::V1::GcmsController < ApplicationController
  respond_to :json
  skip_before_action :verify_authenticity_token

  def create
    email = params["email"]
    reg_token = params["reg_token"]
    user = User.find_by_email(email)
    if user && user.update(reg_token: reg_token)
      render :json => {error: false}
    else
      render :json => {error: true}
    end
  end
end
