class CreateMarkersRoles < ActiveRecord::Migration
  def change
    create_table :markers_roles do |t|

      t.timestamps null: false
    end
  end
end
