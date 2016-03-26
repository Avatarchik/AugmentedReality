class Marker < ActiveRecord::Base
  resourcify
  
  mount_uploader :image, AttachmentUploader
  mount_uploader :iset, AttachmentUploader
  mount_uploader :fset, AttachmentUploader
  mount_uploader :fset3, AttachmentUploader
end
