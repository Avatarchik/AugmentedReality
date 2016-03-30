require 'gcm'

module ApplicationHelper

  def send_message title, body, reg_tokens
    gcm = GCM.new("AIzaSyDj8o0B-plxM2SurQqQOG75OvKpiU4YaSE")

    registration_ids= reg_tokens

    options = { :data => { :title => title, :body => body } }
    response = gcm.send(registration_ids, options)
  end
end
