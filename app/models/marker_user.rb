class MarkerUser < ActiveRecord::Base
  resourcify
  belongs_to :user
  belongs_to :marker
end
