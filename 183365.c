ptp_unpack_Canon_EOS_FE (PTPParams *params, unsigned char* data, PTPCANONFolderEntry *fe)
{
	int i;

	fe->ObjectHandle=dtoh32a(&data[PTP_cefe_ObjectHandle]);
	fe->ObjectFormatCode=dtoh16a(&data[PTP_cefe_ObjectFormatCode]);
	fe->Flags=dtoh8a(&data[PTP_cefe_Flags]);
	fe->ObjectSize=dtoh32a((unsigned char*)&data[PTP_cefe_ObjectSize]);
	fe->Time=(time_t)dtoh32a(&data[PTP_cefe_Time]);
	for (i=0; i<PTP_CANON_FilenameBufferLen; i++)
		fe->Filename[i]=(char)data[PTP_cefe_Filename+i];
}