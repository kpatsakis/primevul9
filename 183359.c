ptp_pack_string(PTPParams *params, char *string, unsigned char* data, uint16_t offset, uint8_t *len)
{
	int packedlen = 0;
	uint16_t ucs2str[PTP_MAXSTRLEN+1];
	char *ucs2strp = (char *) ucs2str;
	size_t convlen = strlen(string);

	/* Cannot exceed 255 (PTP_MAXSTRLEN) since it is a single byte, duh ... */
	memset(ucs2strp, 0, sizeof(ucs2str));  /* XXX: necessary? */
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
	if (params->cd_locale_to_ucs2 != (iconv_t)-1) {
		size_t nconv;
		size_t convmax = PTP_MAXSTRLEN * 2; /* Includes the terminator */
		char *stringp = string;

		nconv = iconv(params->cd_locale_to_ucs2, &stringp, &convlen,
			&ucs2strp, &convmax);
		if (nconv == (size_t) -1)
			ucs2str[0] = 0x0000U;
	} else
#endif
	{
		unsigned int i;

		for (i=0;i<convlen;i++) {
			ucs2str[i] = string[i];
		}
		ucs2str[convlen] = 0;
	}
	/*
	 * XXX: isn't packedlen just ( (uint16_t *)ucs2strp - ucs2str )?
	 *      why do we need ucs2strlen()?
	 */
	packedlen = ucs2strlen(ucs2str);
	if (packedlen > PTP_MAXSTRLEN-1) {
		*len=0;
		return;
	}
	
	/* number of characters including terminating 0 (PTP standard confirmed) */
	htod8a(&data[offset],packedlen+1);
	memcpy(&data[offset+1], &ucs2str[0], packedlen * sizeof(ucs2str[0]));
	htod16a(&data[offset+packedlen*2+1], 0x0000);  /* terminate 0 */

	/* The returned length is in number of characters */
	*len = (uint8_t) packedlen+1;
}