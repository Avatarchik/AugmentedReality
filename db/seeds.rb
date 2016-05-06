admin = User.create(name: "Admin", email: "admin@gmail.com", password: "No01Hien", password_confirmation: "No01Hien")
admin.add_role "admin"
