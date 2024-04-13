static bool is_directory_internal(const path &fpath)
{
	error_code ec;
	bool is_dir = bfs::is_directory(fpath, ec);
	if (error_except_not_found(ec)) {
		LOG_FS << "Failed to check if " << fpath.string() << " is a directory: " << ec.message() << '\n';
	}
	return is_dir;
}