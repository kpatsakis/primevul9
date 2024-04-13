get_default_magic(void)
{
	static const char hmagic[] = "/.magic/magic.mgc";
	static char *default_magic;
	char *home, *hmagicpath;

#ifndef PHP_WIN32
	struct stat st;

	if (default_magic) {
		free(default_magic);
		default_magic = NULL;
	}
	if ((home = getenv("HOME")) == NULL)
		return MAGIC;

	if (asprintf(&hmagicpath, "%s/.magic.mgc", home) < 0)
		return MAGIC;
	if (stat(hmagicpath, &st) == -1) {
		free(hmagicpath);
	if (asprintf(&hmagicpath, "%s/.magic", home) < 0)
		return MAGIC;
	if (stat(hmagicpath, &st) == -1)
		goto out;
	if (S_ISDIR(st.st_mode)) {
		free(hmagicpath);
		if (asprintf(&hmagicpath, "%s/%s", home, hmagic) < 0)
			return MAGIC;
		if (access(hmagicpath, R_OK) == -1)
			goto out;
	}
	}

	if (asprintf(&default_magic, "%s:%s", hmagicpath, MAGIC) < 0)
		goto out;
	free(hmagicpath);
	return default_magic;
out:
	default_magic = NULL;
	free(hmagicpath);
	return MAGIC;
#else
	char *hmagicp = hmagicpath;
	char *tmppath = NULL;
	LPTSTR dllpath;

#define APPENDPATH() \
	do { \
		if (tmppath && access(tmppath, R_OK) != -1) { \
			if (hmagicpath == NULL) \
				hmagicpath = tmppath; \
			else { \
				if (asprintf(&hmagicp, "%s%c%s", hmagicpath, \
				    PATHSEP, tmppath) >= 0) { \
					free(hmagicpath); \
					hmagicpath = hmagicp; \
				} \
				free(tmppath); \
			} \
			tmppath = NULL; \
		} \
	} while (/*CONSTCOND*/0)
				
	if (default_magic) {
		free(default_magic);
		default_magic = NULL;
	}

	/* First, try to get user-specific magic file */
	if ((home = getenv("LOCALAPPDATA")) == NULL) {
		if ((home = getenv("USERPROFILE")) != NULL)
			if (asprintf(&tmppath,
			    "%s/Local Settings/Application Data%s", home,
			    hmagic) < 0)
				tmppath = NULL;
	} else {
		if (asprintf(&tmppath, "%s%s", home, hmagic) < 0)
			tmppath = NULL;
	}

	APPENDPATH();

	/* Second, try to get a magic file from Common Files */
	if ((home = getenv("COMMONPROGRAMFILES")) != NULL) {
		if (asprintf(&tmppath, "%s%s", home, hmagic) >= 0)
			APPENDPATH();
	}

	/* Third, try to get magic file relative to dll location */
	dllpath = malloc(sizeof(*dllpath) * (MAX_PATH + 1));
	dllpath[MAX_PATH] = 0;	/* just in case long path gets truncated and not null terminated */
	if (GetModuleFileNameA(NULL, dllpath, MAX_PATH)){
		PathRemoveFileSpecA(dllpath);
		if (strlen(dllpath) > 3 &&
		    stricmp(&dllpath[strlen(dllpath) - 3], "bin") == 0) {
			if (asprintf(&tmppath,
			    "%s/../share/misc/magic.mgc", dllpath) >= 0)
				APPENDPATH();
		} else {
			if (asprintf(&tmppath,
			    "%s/share/misc/magic.mgc", dllpath) >= 0)
				APPENDPATH();
			else if (asprintf(&tmppath,
			    "%s/magic.mgc", dllpath) >= 0)
				APPENDPATH();
		}
	}

	/* Don't put MAGIC constant - it likely points to a file within MSys
	tree */
	default_magic = hmagicpath;
	return default_magic;
#endif
}