static void set_user_config_path(path newconfig)
{
	user_config_dir = newconfig;
	if (!create_directory_if_missing_recursive(user_config_dir)) {
		ERR_FS << "could not open or create user config directory at " << user_config_dir.string() << '\n';
	}
}