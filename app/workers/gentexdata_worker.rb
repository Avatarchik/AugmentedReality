class GentexdataWorker
  include Sidekiq::Worker
  sidekiq_options retry: false

  def perform(marker_id)
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

    marker.save!

    File.delete(Rails.root.join(iset))
    File.delete(Rails.root.join(fset))
    File.delete(Rails.root.join(fset3))
  end
end
