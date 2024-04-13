bool make_directory(const std::string& dirname)
{
	error_code ec;
	bool created = bfs::create_directory(path(dirname), ec);
	if (ec) {
		ERR_FS << "Failed to create directory " << dirname << ": " << ec.message() << '\n';
	}
	return created;
}