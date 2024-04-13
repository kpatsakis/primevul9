static bool create_directory_if_missing(const path &dirpath)
{
	error_code ec;
	bfs::file_status fs = bfs::status(dirpath, ec);
	if (error_except_not_found(ec)) {
		ERR_FS << "Failed to retrieve file status for " << dirpath.string() << ": " << ec.message() << '\n';
		return false;
	} else if (bfs::is_directory(fs)) {
		DBG_FS << "directory " << dirpath.string() << " exists, not creating\n";
		return true;
	} else if (bfs::exists(fs)) {
		ERR_FS << "cannot create directory " << dirpath.string() << "; file exists\n";
		return false;
	}

	bool created = bfs::create_directory(dirpath, ec);
	if (ec) {
		ERR_FS << "Failed to create directory " << dirpath.string() << ": " << ec.message() << '\n';
	}
	return created;
}