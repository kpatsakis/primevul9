static struct encrypted_key_payload *encrypted_key_alloc(struct key *key,
							 const char *format,
							 const char *master_desc,
							 const char *datalen)
{
	struct encrypted_key_payload *epayload = NULL;
	unsigned short datablob_len;
	unsigned short decrypted_datalen;
	unsigned short payload_datalen;
	unsigned int encrypted_datalen;
	unsigned int format_len;
	long dlen;
	int ret;

	ret = kstrtol(datalen, 10, &dlen);
	if (ret < 0 || dlen < MIN_DATA_SIZE || dlen > MAX_DATA_SIZE)
		return ERR_PTR(-EINVAL);

	format_len = (!format) ? strlen(key_format_default) : strlen(format);
	decrypted_datalen = dlen;
	payload_datalen = decrypted_datalen;
	if (format && !strcmp(format, key_format_ecryptfs)) {
		if (dlen != ECRYPTFS_MAX_KEY_BYTES) {
			pr_err("encrypted_key: keylen for the ecryptfs format "
			       "must be equal to %d bytes\n",
			       ECRYPTFS_MAX_KEY_BYTES);
			return ERR_PTR(-EINVAL);
		}
		decrypted_datalen = ECRYPTFS_MAX_KEY_BYTES;
		payload_datalen = sizeof(struct ecryptfs_auth_tok);
	}

	encrypted_datalen = roundup(decrypted_datalen, blksize);

	datablob_len = format_len + 1 + strlen(master_desc) + 1
	    + strlen(datalen) + 1 + ivsize + 1 + encrypted_datalen;

	ret = key_payload_reserve(key, payload_datalen + datablob_len
				  + HASH_SIZE + 1);
	if (ret < 0)
		return ERR_PTR(ret);

	epayload = kzalloc(sizeof(*epayload) + payload_datalen +
			   datablob_len + HASH_SIZE + 1, GFP_KERNEL);
	if (!epayload)
		return ERR_PTR(-ENOMEM);

	epayload->payload_datalen = payload_datalen;
	epayload->decrypted_datalen = decrypted_datalen;
	epayload->datablob_len = datablob_len;
	return epayload;
}