static rsRetVal setMaxFiles(void __attribute__((unused)) *pVal, int iFiles)
{
	struct rlimit maxFiles;
	char errStr[1024];
	DEFiRet;

	maxFiles.rlim_cur = iFiles;
	maxFiles.rlim_max = iFiles;

	if(setrlimit(RLIMIT_NOFILE, &maxFiles) < 0) {
		/* NOTE: under valgrind, we seem to be unable to extend the size! */
		rs_strerror_r(errno, errStr, sizeof(errStr));
		errmsg.LogError(0, RS_RET_ERR_RLIM_NOFILE, "could not set process file limit to %d: %s [kernel max %ld]",
				iFiles, errStr, (long) maxFiles.rlim_max);
		ABORT_FINALIZE(RS_RET_ERR_RLIM_NOFILE);
	}
	DBGPRINTF("Max number of files set to %d [kernel max %ld].\n", iFiles, (long) maxFiles.rlim_max);

finalize_it:
	RETiRet;
}