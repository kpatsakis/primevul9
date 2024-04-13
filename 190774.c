ConvertPath(char* path)
{
	char p[PATH_MAX];
	memset(p, 0, PATH_MAX);
	
	if( (path == NULL) ||
		(strlen(path) == 0) ||
		(strlen(path)+strlen(ftproot) > PATH_MAX - 1) ) {

		rfbLog("File [%s]: Method [%s]: cannot create path for file transfer\n",
				__FILE__, __FUNCTION__);
		return NULL;
	}

	memcpy(p, path, strlen(path));
	memset(path, 0, PATH_MAX);
	sprintf(path, "%s%s", ftproot, p);

	return path;
}