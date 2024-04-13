std::string get_cwd()
{
	error_code ec;
	path cwd = bfs::current_path(ec);
	if (ec) {
		ERR_FS << "Failed to get current directory: " << ec.message() << '\n';
		return "";
	}
	return cwd.generic_string();
}