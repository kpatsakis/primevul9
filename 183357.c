ptp_unpack_string(PTPParams *params, unsigned char* data, uint16_t offset, uint32_t total, uint8_t *len)
{
	uint8_t length;
	uint16_t string[PTP_MAXSTRLEN+1];
	/* allow for UTF-8: max of 3 bytes per UCS-2 char, plus final null */
	char loclstr[PTP_MAXSTRLEN*3+1];
	size_t nconv, srclen, destlen;
	char *src, *dest;

	if (offset + 1 >= total)
		return NULL;

	length = dtoh8a(&data[offset]);	/* PTP_MAXSTRLEN == 255, 8 bit len */
	*len = length;
	if (length == 0)		/* nothing to do? */
		return NULL;

	if (offset + 1 + length*sizeof(string[0]) > total)
		return NULL;

	/* copy to string[] to ensure correct alignment for iconv(3) */
	memcpy(string, &data[offset+1], length * sizeof(string[0]));
	string[length] = 0x0000U;   /* be paranoid!  add a terminator. */
	loclstr[0] = '\0';

	/* convert from camera UCS-2 to our locale */
	src = (char *)string;
	srclen = length * sizeof(string[0]);
	dest = loclstr;
	destlen = sizeof(loclstr)-1;
	nconv = (size_t)-1;
#if defined(HAVE_ICONV) && defined(HAVE_LANGINFO_H)
	if (params->cd_ucs2_to_locale != (iconv_t)-1)
		nconv = iconv(params->cd_ucs2_to_locale, &src, &srclen, &dest, &destlen);
#endif
	if (nconv == (size_t) -1) { /* do it the hard way */
		int i;
		/* try the old way, in case iconv is broken */
		for (i=0;i<length;i++) {
			if (dtoh16a(&data[offset+1+2*i])>127)
				loclstr[i] = '?';
			else
				loclstr[i] = dtoh16a(&data[offset+1+2*i]);
		}
		dest = loclstr+length;
	}
	*dest = '\0';
	loclstr[sizeof(loclstr)-1] = '\0';   /* be safe? */
	return(strdup(loclstr));
}