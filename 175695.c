static char *datablob_format(struct encrypted_key_payload *epayload,
			     size_t asciiblob_len)
{
	char *ascii_buf, *bufp;
	u8 *iv = epayload->iv;
	int len;
	int i;

	ascii_buf = kmalloc(asciiblob_len + 1, GFP_KERNEL);
	if (!ascii_buf)
		goto out;

	ascii_buf[asciiblob_len] = '\0';

	/* copy datablob master_desc and datalen strings */
	len = sprintf(ascii_buf, "%s %s %s ", epayload->format,
		      epayload->master_desc, epayload->datalen);

	/* convert the hex encoded iv, encrypted-data and HMAC to ascii */
	bufp = &ascii_buf[len];
	for (i = 0; i < (asciiblob_len - len) / 2; i++)
		bufp = hex_byte_pack(bufp, iv[i]);
out:
	return ascii_buf;
}