static path get_dir(const path &dirpath)
{
	bool is_dir = is_directory_internal(dirpath);
	if (!is_dir) {
		error_code ec;
		bfs::create_directory(dirpath, ec);
		if (ec) {
			ERR_FS << "Failed to create directory " << dirpath.string() << ": " << ec.message() << '\n';
		}
		// This is probably redundant
		is_dir = is_directory_internal(dirpath);
	}
	if (!is_dir) {
		ERR_FS << "Could not open or create directory " << dirpath.string() << '\n';
		return std::string();
	}

	return dirpath;
}