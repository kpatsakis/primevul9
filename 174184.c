void binary_paths_manager::set_paths(const config& cfg)
{
	cleanup();
	init_binary_paths();

	BOOST_FOREACH(const config &bp, cfg.child_range("binary_path"))
	{
		std::string path = bp["path"].str();
		if (path.find("..") != std::string::npos) {
			ERR_FS << "Invalid binary path '" << path << "'" << std::endl;
			continue;
		}
		if (!path.empty() && path[path.size()-1] != '/') path += "/";
		if(binary_paths.count(path) == 0) {
			binary_paths.insert(path);
			paths_.push_back(path);
		}
	}
}