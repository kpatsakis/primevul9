int file_size(const std::string& fname)
{
	error_code ec;
	uintmax_t size = bfs::file_size(path(fname), ec);
	if (ec) {
		LOG_FS << "Failed to read filesize of " << fname << ": " << ec.message() << '\n';
		return -1;
	} else if (size > INT_MAX)
		return INT_MAX;
	else
		return size;
}