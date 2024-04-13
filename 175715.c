static int datablob_hmac_append(struct encrypted_key_payload *epayload,
				const u8 *master_key, size_t master_keylen)
{
	u8 derived_key[HASH_SIZE];
	u8 *digest;
	int ret;

	ret = get_derived_key(derived_key, AUTH_KEY, master_key, master_keylen);
	if (ret < 0)
		goto out;

	digest = epayload->format + epayload->datablob_len;
	ret = calc_hmac(digest, derived_key, sizeof derived_key,
			epayload->format, epayload->datablob_len);
	if (!ret)
		dump_hmac(NULL, digest, HASH_SIZE);
out:
	return ret;
}