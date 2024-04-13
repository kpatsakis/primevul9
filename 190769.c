HandleFileUploadFailedRequest(rfbClientPtr cl, rfbTightClientPtr rtcp)
{
	int n = 0;
	char* reason = NULL;
	rfbClientToServerTightMsg msg;

	memset(&msg, 0, sizeof(rfbClientToServerTightMsg));
	
	if(cl == NULL) {
		rfbLog("File [%s]: Method [%s]: Unexpected error: rfbClientPtr is null\n",
				__FILE__, __FUNCTION__);
		return;
	}
	
	if((n = rfbReadExact(cl, ((char *)&msg)+1, sz_rfbFileUploadFailedMsg-1)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileUploadFailedMsg\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}

	msg.fuf.reasonLen = Swap16IfLE(msg.fuf.reasonLen);
	if(msg.fuf.reasonLen  == 0) {
		rfbLog("File [%s]: Method [%s]: reason length received is Zero\n",
				__FILE__, __FUNCTION__);
		return;
	}


	reason = (char*) calloc(msg.fuf.reasonLen + 1, sizeof(char));
	if(reason == NULL) {
		rfbLog("File [%s]: Method [%s]: Memory alloc failed\n", __FILE__, __FUNCTION__);
		return;		
	}
	
	if((n = rfbReadExact(cl, reason, msg.fuf.reasonLen)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileUploadFailedMsg\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);

		if(reason != NULL) {
			free(reason);
			reason = NULL;
		}

	    return;
	}

	rfbLog("File [%s]: Method [%s]: File Upload Failed Request received:"
				" reason <%s>\n", __FILE__, __FUNCTION__, reason);

	CloseUndoneFileTransfer(cl, rtcp);

	if(reason != NULL) {
		free(reason);
		reason = NULL;
	}

}