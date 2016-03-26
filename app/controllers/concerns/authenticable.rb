module Authenticable

  def current_user_api
    @current_user_api ||= User.find_by(auth_token: request.headers['Authorization'])
  end

  def authenticate_with_token!
    render json: { errors: "Not authenticated" },
             status: :unauthorized unless current_user_api.present?
  end
end
