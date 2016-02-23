require 'sidekiq/web'
require 'api_constraints'

Rails.application.routes.draw do

  devise_for :users
  root 'markers#index'
  resources :markers
  mount Sidekiq::Web, at: "/sidekiq"

  namespace :api, defaults: { format: :json } do
    scope module: :v1, constraints: ApiConstraints.new(version: 1, default: true) do
      resources :markers
    end
  end
end
