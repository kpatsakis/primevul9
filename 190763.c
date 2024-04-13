HandleFileUploadDataRequest(rfbClientPtr cl, rfbTightClientPtr rtcp)
{
	int n = 0;
	char* pBuf = NULL;
	rfbClientToServerTightMsg msg;

	memset(&msg, 0, sizeof(rfbClientToServerTightMsg));
	
	if(cl == NULL) {
		rfbLog("File [%s]: Method [%s]: Unexpected error: rfbClientPtr is null\n",
				__FILE__, __FUNCTION__);
		return;
	}

	if((n = rfbReadExact(cl, ((char *)&msg)+1, sz_rfbFileUploadDataMsg-1)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileUploadRequestMsg\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}

	msg.fud.realSize = Swap16IfLE(msg.fud.realSize);
	msg.fud.compressedSize = Swap16IfLE(msg.fud.compressedSize);
	if((msg.fud.realSize == 0) && (msg.fud.compressedSize == 0)) {
		if((n = rfbReadExact(cl, (char*)&(rtcp->rcft.rcfu.mTime), sizeof(unsigned 
		long))) <= 0) {
			
			if (n < 0)
				rfbLog("File [%s]: Method [%s]: Error while reading FileUploadRequestMsg\n",
						__FILE__, __FUNCTION__);
			
		    rfbCloseClient(cl);
		    return;
		}

		FileUpdateComplete(cl, rtcp);
		return;
	}

	pBuf = (char*) calloc(msg.fud.compressedSize, sizeof(char));
	if(pBuf == NULL) {
		rfbLog("File [%s]: Method [%s]: Memory alloc failed\n", __FILE__, __FUNCTION__);
		return;
	}
	if((n = rfbReadExact(cl, pBuf, msg.fud.compressedSize)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Error while reading FileUploadRequestMsg\n",
					__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);

	    if(pBuf != NULL) {
	    	free(pBuf);
	    	pBuf = NULL;
		}
	    
	    return;
	}	
	if(msg.fud.compressedLevel != 0) {
		FileTransferMsg ftm;
		memset(&ftm, 0, sizeof(FileTransferMsg));
		
		ftm = GetFileUploadCompressedLevelErrMsg();

		if((ftm.data != NULL) && (ftm.length != 0)) {
			rfbWriteExact(cl, ftm.data, ftm.length);
			FreeFileTransferMsg(ftm);
		}

		CloseUndoneFileTransfer(cl, rtcp);

	    if(pBuf != NULL) {
	    	free(pBuf);
	    	pBuf = NULL;
		}
		
		return;
	}

	rtcp->rcft.rcfu.fSize = msg.fud.compressedSize;
	
	HandleFileUploadWrite(cl, rtcp, pBuf);

    if(pBuf != NULL) {
    	free(pBuf);
    	pBuf = NULL;
	}

}