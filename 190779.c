HandleFileListRequest(rfbClientPtr cl, rfbTightClientRec* data)
{
	rfbClientToServerTightMsg msg;
	int n = 0;
	char path[PATH_MAX]; /* PATH_MAX has the value 4096 and is defined in limits.h */
	FileTransferMsg fileListMsg;

	memset(&msg, 0, sizeof(rfbClientToServerTightMsg));
	memset(path, 0, PATH_MAX);
	memset(&fileListMsg, 0, sizeof(FileTransferMsg));

	if(cl == NULL) {
		rfbLog("File [%s]: Method [%s]: Unexpected error: rfbClientPtr is null\n", 
				__FILE__, __FUNCTION__);
		return;
	}

	if((n = rfbReadExact(cl, ((char *)&msg)+1, sz_rfbFileListRequestMsg-1)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Socket error while reading dir name"
					" length\n", __FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}
	
	msg.flr.dirNameSize = Swap16IfLE(msg.flr.dirNameSize);
	if ((msg.flr.dirNameSize == 0) ||
		(msg.flr.dirNameSize > (PATH_MAX - 1))) {
		
		rfbLog("File [%s]: Method [%s]: Unexpected error:: path length is "
				"greater that PATH_MAX\n", __FILE__, __FUNCTION__);

		return;		
	}
	
	if((n = rfbReadExact(cl, path, msg.flr.dirNameSize)) <= 0) {
		
		if (n < 0)
			rfbLog("File [%s]: Method [%s]: Socket error while reading dir name\n", 
							__FILE__, __FUNCTION__);
		
	    rfbCloseClient(cl);
	    return;
	}

	if(ConvertPath(path) == NULL) {

		/* The execution should never reach here */
    	rfbLog("File [%s]: Method [%s]: Unexpected error: path is NULL", 
    			__FILE__, __FUNCTION__);
    	return;
	}
	
    fileListMsg = GetFileListResponseMsg(path, (char) (msg.flr.flags));

    if((fileListMsg.data == NULL) || (fileListMsg.length == 0)) {

    	rfbLog("File [%s]: Method [%s]: Unexpected error:: Data to be sent is "
    		"of Zero length\n",	__FILE__, __FUNCTION__);
    	return;
	}	

    rfbWriteExact(cl, fileListMsg.data, fileListMsg.length); 

    FreeFileTransferMsg(fileListMsg);
}