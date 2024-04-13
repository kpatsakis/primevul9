static bool create_directory_if_missing_recursive(const path& dirpath)
{
	DBG_FS << "creating recursive directory: " << dirpath.string() << '\n';

	if (dirpath.empty())
		return false;
	error_code ec;
	bfs::file_status fs = bfs::status(dirpath);
	if (error_except_not_found(ec)) {
		ERR_FS << "Failed to retrieve file status for " << dirpath.string() << ": " << ec.message() << '\n';
		return false;
	} else if (bfs::is_directory(fs)) {
		return true;
	} else if (bfs::exists(fs)) {
		return false;
	}

	if (!dirpath.has_parent_path() || create_directory_if_missing_recursive(dirpath.parent_path())) {
		return create_directory_if_missing(dirpath);
	} else {
		ERR_FS << "Could not create parents to " << dirpath.string() << '\n';
		return false;
	}
}