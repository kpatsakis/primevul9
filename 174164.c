std::string get_user_data_dir()
{
	// ensure setup gets called only once per session
	// FIXME: this is okay and optimized, but how should we react
	// if the user deletes a dir while we are running?
	if (user_data_dir.empty())
	{
		set_user_data_dir(std::string());
	}
	return user_data_dir;
}