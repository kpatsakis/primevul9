ptp_pack_OI (PTPParams *params, PTPObjectInfo *oi, unsigned char** oidataptr)
{
	unsigned char* oidata;
	uint8_t filenamelen;
	uint8_t capturedatelen=0;
	/* let's allocate some memory first; correct assuming zero length dates */
	oidata=malloc(PTP_oi_MaxLen + params->ocs64*4);
	*oidataptr=oidata;
	/* the caller should free it after use! */
#if 0
	char *capture_date="20020101T010101"; /* XXX Fake date */
#endif
	memset (oidata, 0, PTP_oi_MaxLen + params->ocs64*4);
	htod32a(&oidata[PTP_oi_StorageID],oi->StorageID);
	htod16a(&oidata[PTP_oi_ObjectFormat],oi->ObjectFormat);
	htod16a(&oidata[PTP_oi_ProtectionStatus],oi->ProtectionStatus);
	htod32a(&oidata[PTP_oi_ObjectCompressedSize],oi->ObjectCompressedSize);
	if (params->ocs64)
		oidata += 4;
	htod16a(&oidata[PTP_oi_ThumbFormat],oi->ThumbFormat);
	htod32a(&oidata[PTP_oi_ThumbCompressedSize],oi->ThumbCompressedSize);
	htod32a(&oidata[PTP_oi_ThumbPixWidth],oi->ThumbPixWidth);
	htod32a(&oidata[PTP_oi_ThumbPixHeight],oi->ThumbPixHeight);
	htod32a(&oidata[PTP_oi_ImagePixWidth],oi->ImagePixWidth);
	htod32a(&oidata[PTP_oi_ImagePixHeight],oi->ImagePixHeight);
	htod32a(&oidata[PTP_oi_ImageBitDepth],oi->ImageBitDepth);
	htod32a(&oidata[PTP_oi_ParentObject],oi->ParentObject);
	htod16a(&oidata[PTP_oi_AssociationType],oi->AssociationType);
	htod32a(&oidata[PTP_oi_AssociationDesc],oi->AssociationDesc);
	htod32a(&oidata[PTP_oi_SequenceNumber],oi->SequenceNumber);
	
	ptp_pack_string(params, oi->Filename, oidata, PTP_oi_filenamelen, &filenamelen);
/*
	filenamelen=(uint8_t)strlen(oi->Filename);
	htod8a(&req->data[PTP_oi_filenamelen],filenamelen+1);
	for (i=0;i<filenamelen && i< PTP_MAXSTRLEN; i++) {
		req->data[PTP_oi_Filename+i*2]=oi->Filename[i];
	}
*/
	/*
	 *XXX Fake date.
	 * for example Kodak sets Capture date on the basis of EXIF data.
	 * Spec says that this field is from perspective of Initiator.
	 */
#if 0	/* seems now we don't need any data packed in OI dataset... for now ;)*/
	capturedatelen=strlen(capture_date);
	htod8a(&data[PTP_oi_Filename+(filenamelen+1)*2],
		capturedatelen+1);
	for (i=0;i<capturedatelen && i< PTP_MAXSTRLEN; i++) {
		data[PTP_oi_Filename+(i+filenamelen+1)*2+1]=capture_date[i];
	}
	htod8a(&data[PTP_oi_Filename+(filenamelen+capturedatelen+2)*2+1],
		capturedatelen+1);
	for (i=0;i<capturedatelen && i< PTP_MAXSTRLEN; i++) {
		data[PTP_oi_Filename+(i+filenamelen+capturedatelen+2)*2+2]=
		  capture_date[i];
	}
#endif
	/* XXX this function should return dataset length */
	return (PTP_oi_Filename+filenamelen*2+(capturedatelen+1)*3)+params->ocs64*4;
}