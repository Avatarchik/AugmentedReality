class MarkerUser < ActiveRecord::Base
  belongs_to :user
  belongs_to :marker
end
