InitFileTransfer()
{
	char* userHome = NULL;
	uid_t uid = geteuid();

	if(fileTransferInitted)
		return;

	rfbLog("tightvnc-filetransfer/InitFileTransfer\n");
	
	memset(ftproot, 0, sizeof(ftproot));
	
	userHome = GetHomeDir(uid);

	if((userHome != NULL) && (strlen(userHome) != 0)) {
		SetFtpRoot(userHome);
		FreeHomeDir(userHome);
	}
	
	fileTransferEnabled = TRUE;
	fileTransferInitted = TRUE;
}