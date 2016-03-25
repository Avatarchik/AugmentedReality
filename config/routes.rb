require 'sidekiq/web'
require 'api_constraints'

Rails.application.routes.draw do

  get 'users/index'

  devise_for :users, path: :accounts
  root 'markers#index'
  resources :markers, only: :index
  resources :users, only: :index
  mount Sidekiq::Web, at: "/sidekiq"

  namespace :api, defaults: { format: :json } do
    scope module: :v1, constraints: ApiConstraints.new(version: 1, default: true) do
      resources :users, only: [:show, :create]
      resources :markers, only: [:show, :index, :create]
      resources :gcms, only: :create
      resources :sessions
    end
  end
end
