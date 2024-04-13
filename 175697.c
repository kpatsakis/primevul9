static void __ekey_init(struct encrypted_key_payload *epayload,
			const char *format, const char *master_desc,
			const char *datalen)
{
	unsigned int format_len;

	format_len = (!format) ? strlen(key_format_default) : strlen(format);
	epayload->format = epayload->payload_data + epayload->payload_datalen;
	epayload->master_desc = epayload->format + format_len + 1;
	epayload->datalen = epayload->master_desc + strlen(master_desc) + 1;
	epayload->iv = epayload->datalen + strlen(datalen) + 1;
	epayload->encrypted_data = epayload->iv + ivsize + 1;
	epayload->decrypted_data = epayload->payload_data;

	if (!format)
		memcpy(epayload->format, key_format_default, format_len);
	else {
		if (!strcmp(format, key_format_ecryptfs))
			epayload->decrypted_data =
				ecryptfs_get_auth_tok_key((struct ecryptfs_auth_tok *)epayload->payload_data);

		memcpy(epayload->format, format, format_len);
	}

	memcpy(epayload->master_desc, master_desc, strlen(master_desc));
	memcpy(epayload->datalen, datalen, strlen(datalen));
}