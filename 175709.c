static int encrypted_init(struct encrypted_key_payload *epayload,
			  const char *key_desc, const char *format,
			  const char *master_desc, const char *datalen,
			  const char *hex_encoded_iv)
{
	int ret = 0;

	if (format && !strcmp(format, key_format_ecryptfs)) {
		ret = valid_ecryptfs_desc(key_desc);
		if (ret < 0)
			return ret;

		ecryptfs_fill_auth_tok((struct ecryptfs_auth_tok *)epayload->payload_data,
				       key_desc);
	}

	__ekey_init(epayload, format, master_desc, datalen);
	if (!hex_encoded_iv) {
		get_random_bytes(epayload->iv, ivsize);

		get_random_bytes(epayload->decrypted_data,
				 epayload->decrypted_datalen);
	} else
		ret = encrypted_key_decrypt(epayload, format, hex_encoded_iv);
	return ret;
}