const std::vector<std::string>& get_binary_paths(const std::string& type)
{
	const paths_map::const_iterator itor = binary_paths_cache.find(type);
	if(itor != binary_paths_cache.end()) {
		return itor->second;
	}

	if (type.find("..") != std::string::npos) {
		// Not an assertion, as language.cpp is passing user data as type.
		ERR_FS << "Invalid WML type '" << type << "' for binary paths" << std::endl;
		static std::vector<std::string> dummy;
		return dummy;
	}

	std::vector<std::string>& res = binary_paths_cache[type];

	init_binary_paths();

	BOOST_FOREACH(const std::string &path, binary_paths)
	{
		res.push_back(get_user_data_dir() + "/" + path + type + "/");

		if(!game_config::path.empty()) {
			res.push_back(game_config::path + "/" + path + type + "/");
		}
	}

	// not found in "/type" directory, try main directory
	res.push_back(get_user_data_dir() + "/");

	if(!game_config::path.empty())
		res.push_back(game_config::path+"/");

	return res;
}