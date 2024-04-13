std::string get_independent_image_path(const std::string &filename)
{
	std::string full_path = get_binary_file_location("images", filename);

	if(!full_path.empty()) {
		std::string match = get_user_data_dir() + "/";
		if(full_path.find(match) == 0) {
			return full_path.substr(match.size());
		}
		match = game_config::path + "/";
		if(full_path.find(match) == 0) {
			return full_path.substr(match.size());
		}
	}

	return full_path;
}