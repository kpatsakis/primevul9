std::string get_exe_dir()
{
#ifndef _WIN32
	char buf[1024];
	size_t path_size = readlink("/proc/self/exe", buf, sizeof(buf)-1);
	if(path_size == static_cast<size_t>(-1))
		return std::string();
	buf[path_size] = 0;
	return std::string(dirname(buf));
#else
	return get_cwd();
#endif
}