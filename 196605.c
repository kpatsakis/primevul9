join_path_from_pattern(struct glob_pattern **beg)
{
    struct glob_pattern *p;
    char *path = NULL;
    size_t path_len = 0;

    for (p = *beg; p; p = p->next) {
	const char *str;
	switch (p->type) {
	  case RECURSIVE:
	    str = "**";
	    break;
	  case MATCH_DIR:
	    /* append last slash */
	    str = "";
	    break;
	  default:
	    str = p->str;
	    if (!str) continue;
	}
	if (!path) {
	    path_len = strlen(str);
	    path = GLOB_ALLOC_N(char, path_len + 1);
	    memcpy(path, str, path_len);
	    path[path_len] = '\0';
        }
        else {
	    size_t len = strlen(str);
	    char *tmp;
	    tmp = GLOB_REALLOC(path, path_len + len + 2);
	    if (tmp) {
		path = tmp;
		path[path_len++] = '/';
		memcpy(path + path_len, str, len);
		path_len += len;
		path[path_len] = '\0';
	    }
	}
    }
    return path;
}