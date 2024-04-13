HandleFileUploadWrite(rfbClientPtr cl, rfbTightClientPtr rtcp, char* pBuf)
{
	FileTransferMsg ftm;
	memset(&ftm, 0, sizeof(FileTransferMsg));

	ftm = ChkFileUploadWriteErr(cl, rtcp, pBuf);

	if((ftm.data != NULL) && (ftm.length != 0)) {
		rfbWriteExact(cl, ftm.data, ftm.length);
		FreeFileTransferMsg(ftm);
	}
}