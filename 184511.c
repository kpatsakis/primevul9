findInLibpath(const char *libpath, const char *backupPath)
{
	size_t backupPathLen = strlen(backupPath);
	BOOLEAN leadingColon = FALSE;
	BOOLEAN trailingColon = FALSE;
	const char *libpathdir = libpath;

	if (':' == backupPath[0]) {
		/* Shouldn't happen, but this will work if it does */
		leadingColon = TRUE;
	}
	if (':' == backupPath[backupPathLen - 1]) {
		trailingColon = TRUE;
	}

	while (NULL != libpathdir) {
		if (!leadingColon) {
			/* Skip leading ':'s from libpathdir */
			while (':' == libpathdir[0]) {
				libpathdir += 1;
			}
		}
		if (0 == strncmp(libpathdir, backupPath, backupPathLen)) {
			if (('\0' == libpathdir[backupPathLen]) || (':' == libpathdir[backupPathLen]) || trailingColon) {
				return libpathdir;
			}
		}

		/*
		 * The pointer moves forward by one step if a mismatch occurs previously,
		 * otherwise strchr() would repeatedly return the first char ':' of the original
		 * libpathdir if there is no match. Under such circumstance, it would end up
		 * being in an infinite loop (e.g. libpath = ::x:y, path = b:x:y, prefixlen = 1).
		 * The change works even if the first char of libpathdir isn't ':'.
		 */
		libpathdir = strchr(libpathdir + 1, ':');
	}
	return NULL;
}