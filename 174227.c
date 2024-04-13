time_t file_modified_time(const std::string& fname)
{
	error_code ec;
	std::time_t mtime = bfs::last_write_time(path(fname), ec);
	if (ec) {
		LOG_FS << "Failed to read modification time of " << fname << ": " << ec.message() << '\n';
	}
	return mtime;
}