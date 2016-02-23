class MarkerSerializer < ActiveModel::Serializer
  attributes :id, :name, :image, :fset, :fset3, :iset

  def image
    self.object.image_url
  end

  def fset
    self.object.fset_url
  end

  def fset3
    self.object.fset3_url
  end

  def iset
    self.object.iset_url
  end

end
