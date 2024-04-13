HandleFileDownloadLengthError(rfbClientPtr cl, short fNameSize)
{
	char *path = NULL;
	int n = 0;
	
	if((path = (char*) calloc(fNameSize, sizeof(char))) == NULL) {
		rfbLog("File [%s]: Method [%s]: Fatal Error: Alloc failed\n", 
				__FILE__, __FUNCTION__);
		return;
	}
	if((n = rfbReadExact(cl, path, fNameSize)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading dir name\n", 
							__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);

	    if(path != NULL) {
			free(path);
			path = NULL;
		}
	    
	    return;
	}

    if(path != NULL) {
		free(path);
		path = NULL;
	}
    
	SendFileDownloadLengthErrMsg(cl);
}