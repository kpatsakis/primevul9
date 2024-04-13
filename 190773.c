RunFileDownloadThread(void* client)
{
	rfbClientPtr cl = (rfbClientPtr) client;
	rfbTightClientPtr rtcp = rfbGetTightClientData(cl);
	FileTransferMsg fileDownloadMsg;

	if(rtcp == NULL)
		return NULL;

	memset(&fileDownloadMsg, 0, sizeof(FileTransferMsg));
	do {
		pthread_mutex_lock(&fileDownloadMutex);
		fileDownloadMsg = GetFileDownloadResponseMsgInBlocks(cl, rtcp);
		pthread_mutex_unlock(&fileDownloadMutex);
		
		if((fileDownloadMsg.data != NULL) && (fileDownloadMsg.length != 0)) {
			if(rfbWriteExact(cl, fileDownloadMsg.data, fileDownloadMsg.length) < 0)  {
				rfbLog("File [%s]: Method [%s]: Error while writing to socket \n"
						, __FILE__, __FUNCTION__);

				if(cl != NULL) {
			    	rfbCloseClient(cl);
				CloseUndoneFileTransfer(cl, rtcp);
				}
				
				FreeFileTransferMsg(fileDownloadMsg);
				return NULL;
			}
			FreeFileTransferMsg(fileDownloadMsg);
		}
	} while(rtcp->rcft.rcfd.downloadInProgress == TRUE);
	return NULL;
}