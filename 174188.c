bool delete_file(const std::string& path)
{
	bool ret = true;
	if(remove(path.c_str()) != 0 && errno != ENOENT) {
		ERR_FS << "remove(" << path << "): " << strerror(errno) << "\n";
		ret = false;
	}
	return ret;
}