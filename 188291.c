GIT_INLINE(bool) verify_dospath(
	const char *component,
	size_t len,
	const char dospath[3],
	bool trailing_num)
{
	size_t last = trailing_num ? 4 : 3;

	if (len < last || git__strncasecmp(component, dospath, 3) != 0)
		return true;

	if (trailing_num && (component[3] < '1' || component[3] > '9'))
		return true;

	return (len > last &&
		component[last] != '.' &&
		component[last] != ':');
}