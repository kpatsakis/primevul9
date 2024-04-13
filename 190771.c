HandleFileDownloadRequest(rfbClientPtr cl, rfbTightClientPtr rtcp)
{
	int n = 0;
	char path[PATH_MAX]; /* PATH_MAX has the value 4096 and is defined in limits.h */
	rfbClientToServerTightMsg msg;

 	memset(path, 0, sizeof(path));
	memset(&msg, 0, sizeof(rfbClientToServerTightMsg));
	
	if(cl == NULL) {
		
		rfbLog("File [%s]: Method [%s]: Unexpected error:: rfbClientPtr is null\n",
				__FILE__, __FUNCTION__);
		return;
	}

	if((n = rfbReadExact(cl, ((char *)&msg)+1, sz_rfbFileDownloadRequestMsg-1)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading dir name length\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}

	msg.fdr.fNameSize = Swap16IfLE(msg.fdr.fNameSize);
	msg.fdr.position = Swap16IfLE(msg.fdr.position);

	if ((msg.fdr.fNameSize == 0) ||
		(msg.fdr.fNameSize > (PATH_MAX - 1))) {
		
		rfbLog("File [%s]: Method [%s]: Error: path length is greater than"
				" PATH_MAX\n", __FILE__, __FUNCTION__);
		
		HandleFileDownloadLengthError(cl, msg.fdr.fNameSize);
		return;
	}

	if((n = rfbReadExact(cl, rtcp->rcft.rcfd.fName, msg.fdr.fNameSize)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading dir name length\n",
							__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}
	rtcp->rcft.rcfd.fName[msg.fdr.fNameSize] = '\0';

	if(ConvertPath(rtcp->rcft.rcfd.fName) == NULL) {

    	rfbLog("File [%s]: Method [%s]: Unexpected error: path is NULL",
    			__FILE__, __FUNCTION__);

		 
		 /* This condition can come only if the file path is greater than 
		    PATH_MAX. So sending file path length error msg back to client. 
		 */

    	SendFileDownloadLengthErrMsg(cl);
	return;
	}

	HandleFileDownload(cl, rtcp);

}