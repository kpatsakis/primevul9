void set_user_config_dir(std::string path)
{
	user_config_dir = path;
	create_directory_if_missing(user_config_dir);
}