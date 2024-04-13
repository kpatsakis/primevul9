freeBackupLibpath(J9LibpathBackup *libpathBackup)
{
	if (NULL != libpathBackup->fullpath) {
		free(libpathBackup->fullpath);
		libpathBackup->fullpath = NULL;
	}
	libpathBackup->j9prefixLen = 0;
}