require 'gcm'

class GentexdataWorker
  include Sidekiq::Worker
  sidekiq_options retry: false

  def perform(marker_id, marker_user_id)
    marker = Marker.find_by id: marker_id
    path_image = "public" + marker.image_url
    MarkersHelper.genTexData(path_image)
    pre_path = path_image.partition('.').first
    iset = pre_path + ".iset"
    fset = pre_path + ".fset"
    fset3 = pre_path + ".fset3"

    marker.iset = Rails.root.join(iset).open
    marker.fset = Rails.root.join(fset).open
    marker.fset3 = Rails.root.join(fset3).open
    marker_user = MarkerUser.find_by id: marker_user_id
    marker_user.available = true
    marker_user.save!
    marker.save!

    File.delete(Rails.root.join(iset))
    File.delete(Rails.root.join(fset))
    File.delete(Rails.root.join(fset3))

    gcm = GCM.new("AIzaSyDj8o0B-plxM2SurQqQOG75OvKpiU4YaSE")
    registration_ids= User.pluck(:reg_token)
    options = { :data => { :title => "title", :body => "body" } }
    response = gcm.send(registration_ids, options)

  end
end
