class UsersController < ApplicationController
  before_action :authenticate_user!
  load_and_authorize_resource

  def index
    @users = User.all
  end

  def destroy
    @user.destroy
    redirect_to users_path
  end
end
