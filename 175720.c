static long encrypted_read(const struct key *key, char __user *buffer,
			   size_t buflen)
{
	struct encrypted_key_payload *epayload;
	struct key *mkey;
	const u8 *master_key;
	size_t master_keylen;
	char derived_key[HASH_SIZE];
	char *ascii_buf;
	size_t asciiblob_len;
	int ret;

	epayload = dereference_key_locked(key);

	/* returns the hex encoded iv, encrypted-data, and hmac as ascii */
	asciiblob_len = epayload->datablob_len + ivsize + 1
	    + roundup(epayload->decrypted_datalen, blksize)
	    + (HASH_SIZE * 2);

	if (!buffer || buflen < asciiblob_len)
		return asciiblob_len;

	mkey = request_master_key(epayload, &master_key, &master_keylen);
	if (IS_ERR(mkey))
		return PTR_ERR(mkey);

	ret = get_derived_key(derived_key, ENC_KEY, master_key, master_keylen);
	if (ret < 0)
		goto out;

	ret = derived_key_encrypt(epayload, derived_key, sizeof derived_key);
	if (ret < 0)
		goto out;

	ret = datablob_hmac_append(epayload, master_key, master_keylen);
	if (ret < 0)
		goto out;

	ascii_buf = datablob_format(epayload, asciiblob_len);
	if (!ascii_buf) {
		ret = -ENOMEM;
		goto out;
	}

	up_read(&mkey->sem);
	key_put(mkey);

	if (copy_to_user(buffer, ascii_buf, asciiblob_len) != 0)
		ret = -EFAULT;
	kfree(ascii_buf);

	return asciiblob_len;
out:
	up_read(&mkey->sem);
	key_put(mkey);
	return ret;
}