_lou_getTablePath() {
	char searchPath[MAXSTRING];
	char *path;
	char *cp;
	cp = searchPath;
	path = getenv("LOUIS_TABLEPATH");
	if (path != NULL && path[0] != '\0') cp += sprintf(cp, ",%s", path);
	path = lou_getDataPath();
	if (path != NULL && path[0] != '\0')
		cp += sprintf(cp, ",%s%c%s%c%s", path, DIR_SEP, "liblouis", DIR_SEP, "tables");
#ifdef _WIN32
	path = lou_getProgramPath();
	if (path != NULL) {
		if (path[0] != '\0')
			cp += sprintf(cp, ",%s%s", path, "\\share\\liblouis\\tables");
		free(path);
	}
#else
	cp += sprintf(cp, ",%s", TABLESDIR);
#endif
	if (searchPath[0] != '\0')
		return strdup(&searchPath[1]);
	else
		return strdup(".");
}