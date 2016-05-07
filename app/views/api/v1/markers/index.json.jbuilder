json.markers @marker_users do |marker_user|
  json.(marker_user.marker, :id, :name)
  json.image marker_user.marker.image_url
  json.iset marker_user.marker.iset_url
  json.fset marker_user.marker.fset_url
  json.fset3 marker_user.marker.fset3_url
  json.created_at marker_user.marker.created_at.strftime('%d-%m-%Y')
  json.user_name marker_user.user.name
  json.content marker_user.content
  json.available marker_user.available
end
