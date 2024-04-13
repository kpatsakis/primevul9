void parsePath(const xmlChar *path) {
    const xmlChar *cur;

    if (path == NULL)
	return;
    while (*path != 0) {
	if (nbpaths >= MAX_PATHS) {
	    fprintf(stderr, "MAX_PATHS reached: too many paths\n");
	    return;
	}
	cur = path;
	while ((*cur == ' ') || (*cur == PATH_SEPARATOR))
	    cur++;
	path = cur;
	while ((*cur != 0) && (*cur != ' ') && (*cur != PATH_SEPARATOR))
	    cur++;
	if (cur != path) {
	    paths[nbpaths] = xmlStrndup(path, cur - path);
	    if (paths[nbpaths] != NULL)
		nbpaths++;
	    path = cur;
	}
    }
}