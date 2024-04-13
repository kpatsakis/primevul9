HandleFileDownload(rfbClientPtr cl, rfbTightClientPtr rtcp)
{
	pthread_t fileDownloadThread;
	FileTransferMsg fileDownloadMsg;
	
	memset(&fileDownloadMsg, 0, sizeof(FileTransferMsg));
	fileDownloadMsg = ChkFileDownloadErr(cl, rtcp);
	if((fileDownloadMsg.data != NULL) && (fileDownloadMsg.length != 0)) {
		rfbWriteExact(cl, fileDownloadMsg.data, fileDownloadMsg.length);
		FreeFileTransferMsg(fileDownloadMsg);
		return;
	}
	rtcp->rcft.rcfd.downloadInProgress = FALSE;
	rtcp->rcft.rcfd.downloadFD = -1;

	if(pthread_create(&fileDownloadThread, NULL, RunFileDownloadThread, (void*) 
	cl) != 0) {
		FileTransferMsg ftm = GetFileDownLoadErrMsg();
		
		rfbLog("File [%s]: Method [%s]: Download thread creation failed\n",
				__FILE__, __FUNCTION__);
		
		if((ftm.data != NULL) && (ftm.length != 0)) {
			rfbWriteExact(cl, ftm.data, ftm.length);
			FreeFileTransferMsg(ftm);
			return;
		}
				
	}
	
}