json.marker do
  json.(@marker, :id, :name)
  json.image @marker.image_url
  json.iset @marker.iset_url
  json.fset @marker.fset_url
  json.fset3 @marker.fset3_url
  json.created_at @marker.created_at.strftime('%d-%m-%Y')
end
