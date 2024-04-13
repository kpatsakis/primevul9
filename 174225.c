static const path &get_user_data_path()
{
	// TODO:
	// This function is called frequently. The file_exists call may slow things down a lot.
	if (user_data_dir.empty() || !file_exists(user_data_dir))
	{
		set_user_data_dir(std::string());
	}
	return user_data_dir;
}