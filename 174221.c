std::string get_short_wml_path(const std::string &filename)
{
	path full_path(filename);

	path partial = subtract_path(full_path, get_user_data_path() / "data");
	if (!partial.empty())
		return "~" + partial.string();

	partial = subtract_path(full_path, path(game_config::path) / "data");
	if (!partial.empty())
		return partial.string();

	return filename;
}