static int encrypted_instantiate(struct key *key,
				 struct key_preparsed_payload *prep)
{
	struct encrypted_key_payload *epayload = NULL;
	char *datablob = NULL;
	const char *format = NULL;
	char *master_desc = NULL;
	char *decrypted_datalen = NULL;
	char *hex_encoded_iv = NULL;
	size_t datalen = prep->datalen;
	int ret;

	if (datalen <= 0 || datalen > 32767 || !prep->data)
		return -EINVAL;

	datablob = kmalloc(datalen + 1, GFP_KERNEL);
	if (!datablob)
		return -ENOMEM;
	datablob[datalen] = 0;
	memcpy(datablob, prep->data, datalen);
	ret = datablob_parse(datablob, &format, &master_desc,
			     &decrypted_datalen, &hex_encoded_iv);
	if (ret < 0)
		goto out;

	epayload = encrypted_key_alloc(key, format, master_desc,
				       decrypted_datalen);
	if (IS_ERR(epayload)) {
		ret = PTR_ERR(epayload);
		goto out;
	}
	ret = encrypted_init(epayload, key->description, format, master_desc,
			     decrypted_datalen, hex_encoded_iv);
	if (ret < 0) {
		kfree(epayload);
		goto out;
	}

	rcu_assign_keypointer(key, epayload);
out:
	kfree(datablob);
	return ret;
}