HandleFileUpload(rfbClientPtr cl, rfbTightClientPtr rtcp)
{
	FileTransferMsg fileUploadErrMsg;

	memset(&fileUploadErrMsg, 0, sizeof(FileTransferMsg));
	
	rtcp->rcft.rcfu.uploadInProgress = FALSE;
	rtcp->rcft.rcfu.uploadFD = -1;

	fileUploadErrMsg = ChkFileUploadErr(cl, rtcp);
	if((fileUploadErrMsg.data != NULL) && (fileUploadErrMsg.length != 0)) {
		rfbWriteExact(cl, fileUploadErrMsg.data, fileUploadErrMsg.length);
		FreeFileTransferMsg(fileUploadErrMsg);
	}
}