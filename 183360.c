ptp_unpack_Sony_DPD (PTPParams *params, unsigned char* data, PTPDevicePropDesc *dpd, unsigned int dpdlen, unsigned int *poffset)
{
	unsigned int ret;
#if 0
	unsigned int unk1, unk2;
#endif

	memset (dpd, 0, sizeof(*dpd));
	dpd->DevicePropertyCode=dtoh16a(&data[PTP_dpd_Sony_DevicePropertyCode]);
	dpd->DataType=dtoh16a(&data[PTP_dpd_Sony_DataType]);

#if 0
	/* get set ? */
	unk1 = dtoh8a(&data[PTP_dpd_Sony_GetSet]);
	unk2 = dtoh8a(&data[PTP_dpd_Sony_Unknown]);
	ptp_debug (params, "prop 0x%04x, datatype 0x%04x, unk1 %d unk2 %d", dpd->DevicePropertyCode, dpd->DataType, unk1, unk2);
#endif
	dpd->GetSet=1;

	dpd->FormFlag=PTP_DPFF_None;

	*poffset = PTP_dpd_Sony_FactoryDefaultValue;
	ret = ptp_unpack_DPV (params, data, poffset, dpdlen, &dpd->FactoryDefaultValue, dpd->DataType);
	if (!ret) goto outofmemory;
	if ((dpd->DataType == PTP_DTC_STR) && (*poffset == dpdlen))
		return 1;
	ret = ptp_unpack_DPV (params, data, poffset, dpdlen, &dpd->CurrentValue, dpd->DataType);
	if (!ret) goto outofmemory;

	/* if offset==0 then Data Type format is not supported by this
	   code or the Data Type is a string (with two empty strings as
	   values). In both cases Form Flag should be set to 0x00 and FORM is
	   not present. */

	if (*poffset==PTP_dpd_Sony_FactoryDefaultValue)
		return 1;

	dpd->FormFlag=dtoh8a(&data[*poffset]);
	*poffset+=sizeof(uint8_t);

	switch (dpd->FormFlag) {
	case PTP_DPFF_Range:
		ret = ptp_unpack_DPV (params, data, poffset, dpdlen, &dpd->FORM.Range.MinimumValue, dpd->DataType);
		if (!ret) goto outofmemory;
		ret = ptp_unpack_DPV (params, data, poffset, dpdlen, &dpd->FORM.Range.MaximumValue, dpd->DataType);
		if (!ret) goto outofmemory;
		ret = ptp_unpack_DPV (params, data, poffset, dpdlen, &dpd->FORM.Range.StepSize, dpd->DataType);
		if (!ret) goto outofmemory;
		break;
	case PTP_DPFF_Enumeration: {
		int i;
#define N	dpd->FORM.Enum.NumberOfValues
		N = dtoh16a(&data[*poffset]);
		*poffset+=sizeof(uint16_t);
		dpd->FORM.Enum.SupportedValue = malloc(N*sizeof(dpd->FORM.Enum.SupportedValue[0]));
		if (!dpd->FORM.Enum.SupportedValue)
			goto outofmemory;

		memset (dpd->FORM.Enum.SupportedValue,0 , N*sizeof(dpd->FORM.Enum.SupportedValue[0]));
		for (i=0;i<N;i++) {
			ret = ptp_unpack_DPV (params, data, poffset, dpdlen, &dpd->FORM.Enum.SupportedValue[i], dpd->DataType);

			/* Slightly different handling here. The HP PhotoSmart 120
			 * specifies an enumeration with N in wrong endian
			 * 00 01 instead of 01 00, so we count the enum just until the
			 * the end of the packet.
			 */
			if (!ret) {
				if (!i)
					goto outofmemory;
				dpd->FORM.Enum.NumberOfValues = i;
				break;
			}
		}
		}
	}
#undef N
	return 1;
outofmemory:
	ptp_free_devicepropdesc(dpd);
	return 0;
}