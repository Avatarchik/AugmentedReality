require 'sidekiq/web'
require 'api_constraints'

Rails.application.routes.draw do

  root 'markers#index'
  devise_for :users, path: :accounts
  mount Sidekiq::Web, at: "/sidekiq"

  resources :users, only: [:index, :destroy]
  resources :markers, only: [:index, :new, :create, :destroy]
  resources :notifications, only: [:index, :create]

  namespace :api, defaults: { format: :json } do
    scope module: :v1, constraints: ApiConstraints.new(version: 1, default: true) do
      resources :users, only: [:show, :create]
      resources :markers, only: [:show, :index, :create]
      resources :gcms, only: :create
      resources :sessions
    end
  end
end
