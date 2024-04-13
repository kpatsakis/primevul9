static int get_derived_key(u8 *derived_key, enum derived_key_type key_type,
			   const u8 *master_key, size_t master_keylen)
{
	u8 *derived_buf;
	unsigned int derived_buf_len;
	int ret;

	derived_buf_len = strlen("AUTH_KEY") + 1 + master_keylen;
	if (derived_buf_len < HASH_SIZE)
		derived_buf_len = HASH_SIZE;

	derived_buf = kzalloc(derived_buf_len, GFP_KERNEL);
	if (!derived_buf)
		return -ENOMEM;

	if (key_type)
		strcpy(derived_buf, "AUTH_KEY");
	else
		strcpy(derived_buf, "ENC_KEY");

	memcpy(derived_buf + strlen(derived_buf) + 1, master_key,
	       master_keylen);
	ret = calc_hash(derived_key, derived_buf, derived_buf_len);
	kfree(derived_buf);
	return ret;
}