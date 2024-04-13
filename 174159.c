std::string get_binary_dir_location(const std::string &type, const std::string &filename)
{
	if (!is_legal_file(filename))
		return std::string();

	BOOST_FOREACH(const std::string &bp, get_binary_paths(type))
	{
		path bpath(bp);
		bpath /= filename;
		DBG_FS << "  checking '" << bp << "'\n";
		if (is_directory_internal(bpath)) {
			DBG_FS << "  found at '" << bpath.string() << "'\n";
			return bpath.string();
		}
	}

	DBG_FS << "  not found\n";
	return std::string();
}