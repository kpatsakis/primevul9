int dir_size(const std::string& pname)
{
	bfs::path p(pname);
	uintmax_t size_sum = 0;
	error_code ec;
	for ( bfs::recursive_directory_iterator i(p), end; i != end && !ec; ++i ) {
		if(bfs::is_regular_file(i->path())) {
			size_sum += bfs::file_size(i->path(), ec);
		}
	}
	if (ec) {
		LOG_FS << "Failed to read directorysize of " << pname << ": " << ec.message() << '\n';
		return -1;
	}
	else if (size_sum > INT_MAX)
		return INT_MAX;
	else
		return size_sum;
}