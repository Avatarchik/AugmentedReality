Rails.application.routes.draw do

  root 'markers#index'
  resources :markers
end
