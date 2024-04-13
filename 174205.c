bool delete_file(const std::string &filename)
{
	error_code ec;
	bool ret = bfs::remove(path(filename), ec);
	if (ec) {
		ERR_FS << "Could not delete file " << filename << ": " << ec.message() << '\n';
	}
	return ret;
}