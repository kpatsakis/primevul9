std::string get_independent_image_path(const std::string &filename)
{
	path full_path(get_binary_file_location("images", filename));

	if (full_path.empty())
		return full_path.string();

	path partial = subtract_path(full_path, get_user_data_path());
	if (!partial.empty())
		return partial.string();

	partial = subtract_path(full_path, game_config::path);
	if (!partial.empty())
		return partial.string();

	return full_path.string();
}