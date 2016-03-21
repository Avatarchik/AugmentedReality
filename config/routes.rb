require 'sidekiq/web'
require 'api_constraints'

Rails.application.routes.draw do

  get 'users/index'

  devise_for :users, path: :accounts
  root 'markers#index'
  resources :markers
  resources :users, only: :index
  mount Sidekiq::Web, at: "/sidekiq"

  namespace :api, defaults: { format: :json } do
    scope module: :v1, constraints: ApiConstraints.new(version: 1, default: true) do
      resources :markers, only: [:show, :index, :create]
      resources :registers, only: :create
      resources :logins, only: :create
      resources :gcms, only: :create
    end
  end
end
