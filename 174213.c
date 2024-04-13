std::string get_exe_dir()
{
#ifndef _WIN32
	path self_exe("/proc/self/exe");
	error_code ec;
	path exe = bfs::read_symlink(self_exe, ec);
	if (ec) {
		return std::string();
	}

	return exe.parent_path().string();
#else
	return get_cwd();
#endif
}