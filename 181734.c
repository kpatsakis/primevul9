static char *getViewerPath() {
	int i;
	const char *viewers[] = {
#if __WINDOWS__
		"explorer",
#else
		"open",
		"geeqie",
		"gqview",
		"eog",
		"xdg-open",
#endif
		NULL
	};
	for (i = 0; viewers[i]; i++) {
		char *viewerPath = r_file_path (viewers[i]);
		if (viewerPath && strcmp (viewerPath, viewers[i])) {
			return viewerPath;
		}
		free (viewerPath);
	}
	return NULL;
}