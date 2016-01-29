class CreateMarkers < ActiveRecord::Migration
  def change
    create_table :markers do |t|
      t.string :name
      t.string :image
      t.string :iset
      t.string :fset
      t.string :fset3

      t.timestamps null: false
    end
  end
end
