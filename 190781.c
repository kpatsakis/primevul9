HandleFileCreateDirRequest(rfbClientPtr cl, rfbTightClientPtr rtcp)
{
	int n = 0;
	char dirName[PATH_MAX];
	rfbClientToServerTightMsg msg;

	memset(dirName, 0, PATH_MAX);
	memset(&msg, 0, sizeof(rfbClientToServerTightMsg));
	
	if(cl == NULL) {
		rfbLog("File [%s]: Method [%s]: Unexpected error: rfbClientPtr is null\n",
				__FILE__, __FUNCTION__);
		return;
	}
	
	if((n = rfbReadExact(cl, ((char *)&msg)+1, sz_rfbFileCreateDirRequestMsg-1)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileCreateDirRequestMsg\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}

	msg.fcdr.dNameLen = Swap16IfLE(msg.fcdr.dNameLen);

	/* TODO :: chk if the dNameLen is greater than PATH_MAX */	
	
	if((n = rfbReadExact(cl, dirName, msg.fcdr.dNameLen)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileUploadFailedMsg\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}

	if(ConvertPath(dirName) == NULL) {
    	rfbLog("File [%s]: Method [%s]: Unexpected error: path is NULL\n",
    			__FILE__, __FUNCTION__);

    	return;
	}

	CreateDirectory(dirName);
}