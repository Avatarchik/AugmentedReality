class CreateMarkerUsers < ActiveRecord::Migration
  def change
    create_table :marker_users do |t|
      t.references :user, index: true, foreign_key: true
      t.references :marker, index: true, foreign_key: true
      t.string :content
      t.boolean :available, :default => false

      t.timestamps null: false
    end
  end
end
