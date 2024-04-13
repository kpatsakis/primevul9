static int datablob_hmac_verify(struct encrypted_key_payload *epayload,
				const u8 *format, const u8 *master_key,
				size_t master_keylen)
{
	u8 derived_key[HASH_SIZE];
	u8 digest[HASH_SIZE];
	int ret;
	char *p;
	unsigned short len;

	ret = get_derived_key(derived_key, AUTH_KEY, master_key, master_keylen);
	if (ret < 0)
		goto out;

	len = epayload->datablob_len;
	if (!format) {
		p = epayload->master_desc;
		len -= strlen(epayload->format) + 1;
	} else
		p = epayload->format;

	ret = calc_hmac(digest, derived_key, sizeof derived_key, p, len);
	if (ret < 0)
		goto out;
	ret = memcmp(digest, epayload->format + epayload->datablob_len,
		     sizeof digest);
	if (ret) {
		ret = -EINVAL;
		dump_hmac("datablob",
			  epayload->format + epayload->datablob_len,
			  HASH_SIZE);
		dump_hmac("calc", digest, HASH_SIZE);
	}
out:
	return ret;
}