TSRM_API int win32_utime(const char *filename, struct utimbuf *buf) /* {{{ */
{
	FILETIME mtime, atime;
	HANDLE hFile;

	hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL,
				 OPEN_ALWAYS, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	/* OPEN_ALWAYS mode sets the last error to ERROR_ALREADY_EXISTS but
	   the CreateFile operation succeeds */
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		SetLastError(0);
	}

	if ( hFile == INVALID_HANDLE_VALUE ) {
		return -1;
	}

	if (!buf) {
		SYSTEMTIME st;
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &mtime);
		atime = mtime;
	} else {
		UnixTimeToFileTime(buf->modtime, &mtime);
		UnixTimeToFileTime(buf->actime, &atime);
	}
	if (!SetFileTime(hFile, NULL, &atime, &mtime)) {
		CloseHandle(hFile);
		return -1;
	}
	CloseHandle(hFile);
	return 1;
}