class NotificationsController < ApplicationController
  include ApplicationHelper

  def index

  end

  def create
    reg_tokens = User.pluck(:reg_token)
    send_message(params[:title], params[:body], reg_tokens)
    redirect_to notifications_path
  end
end
