restoreLibpath(J9LibpathBackup *libpathBackup)
{
	if ((NULL != libpathBackup->fullpath) && (libpathBackup->j9prefixLen > 0)) {
		const char *curPath = getenv("LIBPATH");

		if (NULL != curPath) {
			const char *j9pathLoc = findInLibpath(curPath, libpathBackup->fullpath);

			if (NULL != j9pathLoc) {
				char *newPath = deleteDirsFromLibpath(curPath, j9pathLoc, libpathBackup->j9prefixLen);

#ifdef DEBUG
				printf("restoreLibpath: old LIBPATH = <%s>\n", curPath);
				printf("restoreLibpath: new LIBPATH = <%s>\n", newPath);
#endif
				setLibpath(newPath);
				free(newPath);
			}
		}

		free(libpathBackup->fullpath);
		libpathBackup->fullpath = NULL;
		libpathBackup->j9prefixLen = 0;
	}
}