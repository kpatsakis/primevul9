ptp_unpack_OI (PTPParams *params, unsigned char* data, PTPObjectInfo *oi, unsigned int len)
{
	uint8_t filenamelen;
	uint8_t capturedatelen;
	char *capture_date;

	if (len < PTP_oi_SequenceNumber)
		return;

	oi->Filename = oi->Keywords = NULL;

	/* FIXME: also handle length with all the strings at the end */
	oi->StorageID=dtoh32a(&data[PTP_oi_StorageID]);
	oi->ObjectFormat=dtoh16a(&data[PTP_oi_ObjectFormat]);
	oi->ProtectionStatus=dtoh16a(&data[PTP_oi_ProtectionStatus]);
	oi->ObjectCompressedSize=dtoh32a(&data[PTP_oi_ObjectCompressedSize]);

	/* Stupid Samsung Galaxy developers emit a 64bit objectcompressedsize */
	if ((data[PTP_oi_filenamelen] == 0) && (data[PTP_oi_filenamelen+4] != 0)) {
		params->ocs64 = 1;
		data += 4;
	}
	oi->ThumbFormat=dtoh16a(&data[PTP_oi_ThumbFormat]);
	oi->ThumbCompressedSize=dtoh32a(&data[PTP_oi_ThumbCompressedSize]);
	oi->ThumbPixWidth=dtoh32a(&data[PTP_oi_ThumbPixWidth]);
	oi->ThumbPixHeight=dtoh32a(&data[PTP_oi_ThumbPixHeight]);
	oi->ImagePixWidth=dtoh32a(&data[PTP_oi_ImagePixWidth]);
	oi->ImagePixHeight=dtoh32a(&data[PTP_oi_ImagePixHeight]);
	oi->ImageBitDepth=dtoh32a(&data[PTP_oi_ImageBitDepth]);
	oi->ParentObject=dtoh32a(&data[PTP_oi_ParentObject]);
	oi->AssociationType=dtoh16a(&data[PTP_oi_AssociationType]);
	oi->AssociationDesc=dtoh32a(&data[PTP_oi_AssociationDesc]);
	oi->SequenceNumber=dtoh32a(&data[PTP_oi_SequenceNumber]);

	oi->Filename= ptp_unpack_string(params, data, PTP_oi_filenamelen, len, &filenamelen);

	capture_date = ptp_unpack_string(params, data,
		PTP_oi_filenamelen+filenamelen*2+1, len, &capturedatelen);
	/* subset of ISO 8601, without '.s' tenths of second and 
	 * time zone
	 */
	oi->CaptureDate = ptp_unpack_PTPTIME(capture_date);
	free(capture_date);

	/* now the modification date ... */
	capture_date = ptp_unpack_string(params, data,
		PTP_oi_filenamelen+filenamelen*2
		+capturedatelen*2+2, len, &capturedatelen);
	oi->ModificationDate = ptp_unpack_PTPTIME(capture_date);
	free(capture_date);
}