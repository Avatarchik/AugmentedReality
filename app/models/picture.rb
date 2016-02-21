class Picture < ActiveRecord::Base
  mount_uploader :file, FileUploader
end
