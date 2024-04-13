static void setup_user_data_dir()
{
	if (!create_directory_if_missing_recursive(user_data_dir)) {
		ERR_FS << "could not open or create user data directory at " << user_data_dir.string() << '\n';
		return;
	}
	// TODO: this may not print the error message if the directory exists but we don't have the proper permissions

	// Create user data and add-on directories
	create_directory_if_missing(user_data_dir / "editor");
	create_directory_if_missing(user_data_dir / "editor" / "maps");
	create_directory_if_missing(user_data_dir / "editor" / "scenarios");
	create_directory_if_missing(user_data_dir / "data");
	create_directory_if_missing(user_data_dir / "data" / "add-ons");
	create_directory_if_missing(user_data_dir / "saves");
	create_directory_if_missing(user_data_dir / "persist");
}