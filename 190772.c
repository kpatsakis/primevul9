SendFileUploadLengthErrMsg(rfbClientPtr cl)
{

	FileTransferMsg fileUploadErrMsg;
	
	memset(&fileUploadErrMsg, 0, sizeof(FileTransferMsg));
	fileUploadErrMsg = GetFileUploadLengthErrResponseMsg();

	if((fileUploadErrMsg.data == NULL) || (fileUploadErrMsg.length == 0)) {
		rfbLog("File [%s]: Method [%s]: Unexpected error: fileUploadErrMsg is null\n",
				__FILE__, __FUNCTION__);
		return;
	}

	rfbWriteExact(cl, fileUploadErrMsg.data, fileUploadErrMsg.length);
	FreeFileTransferMsg(fileUploadErrMsg);
}