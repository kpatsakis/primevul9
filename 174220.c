std::string get_binary_file_location(const std::string& type, const std::string& filename)
{
	// We define ".." as "remove everything before" this is needed becasue 
	// on the one hand allowing ".." would be a security risk but 
	// especialy for terrains the c++ engine puts a hardcoded "terrain/" before filename
	// and there would be no way to "escape" from "terrain/" otherwise. This is not the 
	// best solution but we cannot remove it without another solution (subtypes maybe?).
	
	// using 'for' instead 'if' to allow putting delcaration and check into the brackets
	for(std::string::size_type pos = filename.rfind("../"); pos != std::string::npos;)
		return get_binary_file_location(type, filename.substr(pos + 3));

	if (!is_legal_file(filename))
		return std::string();

	BOOST_FOREACH(const std::string &bp, get_binary_paths(type))
	{
		path bpath(bp);
		bpath /= filename;
		DBG_FS << "  checking '" << bp << "'\n";
		if (file_exists(bpath)) {
			DBG_FS << "  found at '" << bpath.string() << "'\n";
			return bpath.string();
		}
	}

	DBG_FS << "  not found\n";
	return std::string();
}