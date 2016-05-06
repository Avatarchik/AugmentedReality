admin = User.create(name: "Admin", email: "admin@gmail.com", password: "11111111", password_confirmation: "11111111")
admin.add_role "admin"
