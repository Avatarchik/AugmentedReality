require 'gcm'

module ApplicationHelper
  def send_message
    gcm = GCM.new("AIzaSyDj8o0B-plxM2SurQqQOG75OvKpiU4YaSE")

    registration_ids= [ User.last.reg_token]

    options = { :data => { :title =>"acb", :body => "this is a longer dfsdfd" } }
    response = gcm.send(registration_ids, options)
  end
end
