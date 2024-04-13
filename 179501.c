guid_to_str_buf(const e_guid_t *guid, gchar *buf, int buf_len)
{
	char *tempptr = buf;

	if (buf_len < GUID_STR_LEN) {
		g_strlcpy(buf, BUF_TOO_SMALL_ERR, buf_len);/* Let the unexpected value alert user */
		return buf;
	}

	/* 37 bytes */
	tempptr    = dword_to_hex(tempptr, guid->data1);		/*  8 bytes */
	*tempptr++ = '-';						/*  1 byte */
	tempptr    = word_to_hex(tempptr, guid->data2);			/*  4 bytes */
	*tempptr++ = '-';						/*  1 byte */
	tempptr    = word_to_hex(tempptr, guid->data3);			/*  4 bytes */
	*tempptr++ = '-';						/*  1 byte */
	tempptr    = bytes_to_hexstr(tempptr, &guid->data4[0], 2);	/*  4 bytes */
	*tempptr++ = '-';						/*  1 byte */
	tempptr    = bytes_to_hexstr(tempptr, &guid->data4[2], 6);	/* 12 bytes */

	*tempptr   = '\0';
	return buf;
}