std::string normalize_path(const std::string &fpath)
{
	if (fpath.empty()) {
		return fpath;
	}

	return bfs::absolute(fpath).string();
}