SendFileDownloadLengthErrMsg(rfbClientPtr cl)
{
	FileTransferMsg fileDownloadErrMsg;
	
	memset(&fileDownloadErrMsg, 0 , sizeof(FileTransferMsg));

	fileDownloadErrMsg = GetFileDownloadLengthErrResponseMsg();

	if((fileDownloadErrMsg.data == NULL) || (fileDownloadErrMsg.length == 0)) {
		rfbLog("File [%s]: Method [%s]: Unexpected error: fileDownloadErrMsg "
				"is null\n", __FILE__, __FUNCTION__);
		return;
	}

	rfbWriteExact(cl, fileDownloadErrMsg.data, fileDownloadErrMsg.length);

	FreeFileTransferMsg(fileDownloadErrMsg);
}