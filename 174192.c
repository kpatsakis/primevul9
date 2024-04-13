static bool file_exists(const path &fpath)
{
	error_code ec;
	bool exists = bfs::exists(fpath, ec);
	if (error_except_not_found(ec)) {
		ERR_FS << "Failed to check existence of file " << fpath.string() << ": " << ec.message() << '\n';
	}
	return exists;
}