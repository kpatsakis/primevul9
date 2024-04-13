ptp_unpack_OPD (PTPParams *params, unsigned char* data, PTPObjectPropDesc *opd, unsigned int opdlen)
{
	unsigned int offset=0, ret;

	memset (opd, 0, sizeof(*opd));

	if (opdlen < 5)
		return 0;

	opd->ObjectPropertyCode=dtoh16a(&data[PTP_opd_ObjectPropertyCode]);
	opd->DataType=dtoh16a(&data[PTP_opd_DataType]);
	opd->GetSet=dtoh8a(&data[PTP_opd_GetSet]);

	offset = PTP_opd_FactoryDefaultValue;
	ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FactoryDefaultValue, opd->DataType);
	if (!ret) goto outofmemory;

	opd->GroupCode=dtoh32a(&data[offset]);
	offset+=sizeof(uint32_t);

	opd->FormFlag=dtoh8a(&data[offset]);
	offset+=sizeof(uint8_t);

	switch (opd->FormFlag) {
	case PTP_OPFF_Range:
		ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Range.MinimumValue, opd->DataType);
		if (!ret) goto outofmemory;
		ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Range.MaximumValue, opd->DataType);
		if (!ret) goto outofmemory;
		ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Range.StepSize, opd->DataType);
		if (!ret) goto outofmemory;
		break;
	case PTP_OPFF_Enumeration: {
		unsigned int i;
#define N	opd->FORM.Enum.NumberOfValues
		N = dtoh16a(&data[offset]);
		offset+=sizeof(uint16_t);
		opd->FORM.Enum.SupportedValue = malloc(N*sizeof(opd->FORM.Enum.SupportedValue[0]));
		if (!opd->FORM.Enum.SupportedValue)
			goto outofmemory;

		memset (opd->FORM.Enum.SupportedValue,0 , N*sizeof(opd->FORM.Enum.SupportedValue[0]));
		for (i=0;i<N;i++) {
			ret = ptp_unpack_DPV (params, data, &offset, opdlen, &opd->FORM.Enum.SupportedValue[i], opd->DataType);

			/* Slightly different handling here. The HP PhotoSmart 120
			 * specifies an enumeration with N in wrong endian
			 * 00 01 instead of 01 00, so we count the enum just until the
			 * the end of the packet.
			 */
			if (!ret) {
				if (!i)
					goto outofmemory;
				opd->FORM.Enum.NumberOfValues = i;
				break;
			}
		}
#undef N
		}
	}
	return 1;
outofmemory:
	ptp_free_objectpropdesc(opd);
	return 0;
}