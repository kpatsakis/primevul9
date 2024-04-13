static bool fcnNeedsPrefix(const char *name) {
	if (!strncmp (name, "entry", 5)) {
		return false;
	}
	if (!strncmp (name, "main", 4)) {
		return false;
	}
	return (!strchr (name, '.'));
}