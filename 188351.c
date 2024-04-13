static bool looks_like_network_computer_name(const char *path, int pos)
{
	if (pos < 3)
		return false;

	if (path[0] != '/' || path[1] != '/')
		return false;

	while (pos-- > 2) {
		if (path[pos] == '/')
			return false;
	}

	return true;
}