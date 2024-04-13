HandleFileUploadRequest(rfbClientPtr cl, rfbTightClientPtr rtcp)
{
	int n = 0;
	char path[PATH_MAX]; /* PATH_MAX has the value 4096 and is defined in limits.h */
	rfbClientToServerTightMsg msg;

	memset(path, 0, PATH_MAX);
	memset(&msg, 0, sizeof(rfbClientToServerTightMsg));
	
	if(cl == NULL) {
		rfbLog("File [%s]: Method [%s]: Unexpected error: rfbClientPtr is null\n",
				__FILE__, __FUNCTION__);
		return;
	}
	
	if((n = rfbReadExact(cl, ((char *)&msg)+1, sz_rfbFileUploadRequestMsg-1)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileUploadRequestMsg\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}

	msg.fupr.fNameSize = Swap16IfLE(msg.fupr.fNameSize);
	msg.fupr.position = Swap16IfLE(msg.fupr.position);

	if ((msg.fupr.fNameSize == 0) ||
		(msg.fupr.fNameSize > (PATH_MAX - 1))) {
		
		rfbLog("File [%s]: Method [%s]: error: path length is greater than PATH_MAX\n",
				__FILE__, __FUNCTION__);
		HandleFileUploadLengthError(cl, msg.fupr.fNameSize);
		return;
	}

	if((n = rfbReadExact(cl, rtcp->rcft.rcfu.fName, msg.fupr.fNameSize)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileUploadRequestMsg\n"
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}
	rtcp->rcft.rcfu.fName[msg.fupr.fNameSize] = '\0';
	
	if(ConvertPath(rtcp->rcft.rcfu.fName) == NULL) {
    	rfbLog("File [%s]: Method [%s]: Unexpected error: path is NULL\n",
    			__FILE__, __FUNCTION__);

    	/* This may come if the path length exceeds PATH_MAX.
    	   So sending path length error to client
    	 */
    	 SendFileUploadLengthErrMsg(cl);
    	return;
	}

	HandleFileUpload(cl, rtcp);
}