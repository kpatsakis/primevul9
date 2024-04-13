SetFtpRoot(char* path)
{
	struct stat stat_buf;
	DIR* dir = NULL;

	rfbLog("tightvnc-filetransfer/SetFtpRoot\n");
	
	if((path == NULL) || (strlen(path) == 0) || (strlen(path) > (PATH_MAX - 1))) {
		rfbLog("File [%s]: Method [%s]: parameter passed is improper, ftproot"
				" not changed\n", __FILE__, __FUNCTION__);
		return FALSE;
	}

	if(stat(path, &stat_buf) < 0) {
		rfbLog("File [%s]: Method [%s]: Reading stat for file %s failed\n", 
				__FILE__, __FUNCTION__, path);
		return FALSE;
	}

	if(S_ISDIR(stat_buf.st_mode) == 0) {
		rfbLog("File [%s]: Method [%s]: path specified is not a directory\n",
				__FILE__, __FUNCTION__);
		return FALSE;		
	}

	if((dir = opendir(path)) == NULL) {
		rfbLog("File [%s]: Method [%s]: Not able to open the directory\n",
				__FILE__, __FUNCTION__);
		return FALSE;			
	}
	else {
		closedir(dir);
		dir = NULL;
	}
	
	
	memset(ftproot, 0, PATH_MAX);
	if(path[strlen(path)-1] == '/') {
		memcpy(ftproot, path, strlen(path)-1);	
	}
	else	
		memcpy(ftproot, path, strlen(path));	

	
	return TRUE;
}