backupLibpath(J9LibpathBackup *libpathBackup, size_t origLibpathLen)
{
	const char *curLibpath = getenv("LIBPATH");

	libpathBackup->fullpath = NULL;
	libpathBackup->j9prefixLen = 0;

	if (NULL != curLibpath) {
		/* If j9prefix is not at the end of LIBPATH, then trailing ':' is included in j9prefixLen */
		libpathBackup->j9prefixLen = strlen(curLibpath) - origLibpathLen;

		/* If j9prefixLen == 0, there is no VM prefix to remove. We don't
		 * need to save a copy of the LIBPATH because we don't need to
		 * do any processing in restoreLibpath().
		 */
		if (libpathBackup->j9prefixLen > 0) {
			libpathBackup->fullpath = strdup(curLibpath);
			if (NULL == libpathBackup->fullpath) {
				fprintf(stderr, "backupLibpath: strdup() failed to allocate memory for the backup path\n");
				abort();
			}
		}
	}
}