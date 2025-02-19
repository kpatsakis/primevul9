static int encrypted_update(struct key *key, struct key_preparsed_payload *prep)
{
	struct encrypted_key_payload *epayload = key->payload.data[0];
	struct encrypted_key_payload *new_epayload;
	char *buf;
	char *new_master_desc = NULL;
	const char *format = NULL;
	size_t datalen = prep->datalen;
	int ret = 0;

	if (test_bit(KEY_FLAG_NEGATIVE, &key->flags))
		return -ENOKEY;
	if (datalen <= 0 || datalen > 32767 || !prep->data)
		return -EINVAL;

	buf = kmalloc(datalen + 1, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	buf[datalen] = 0;
	memcpy(buf, prep->data, datalen);
	ret = datablob_parse(buf, &format, &new_master_desc, NULL, NULL);
	if (ret < 0)
		goto out;

	ret = valid_master_desc(new_master_desc, epayload->master_desc);
	if (ret < 0)
		goto out;

	new_epayload = encrypted_key_alloc(key, epayload->format,
					   new_master_desc, epayload->datalen);
	if (IS_ERR(new_epayload)) {
		ret = PTR_ERR(new_epayload);
		goto out;
	}

	__ekey_init(new_epayload, epayload->format, new_master_desc,
		    epayload->datalen);

	memcpy(new_epayload->iv, epayload->iv, ivsize);
	memcpy(new_epayload->payload_data, epayload->payload_data,
	       epayload->payload_datalen);

	rcu_assign_keypointer(key, new_epayload);
	call_rcu(&epayload->rcu, encrypted_rcu_free);
out:
	kfree(buf);
	return ret;
}