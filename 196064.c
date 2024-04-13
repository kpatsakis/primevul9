long keyctl_instantiate_key_common(key_serial_t id,
				   struct iov_iter *from,
				   key_serial_t ringid)
{
	const struct cred *cred = current_cred();
	struct request_key_auth *rka;
	struct key *instkey, *dest_keyring;
	size_t plen = from ? iov_iter_count(from) : 0;
	void *payload;
	long ret;

	kenter("%d,,%zu,%d", id, plen, ringid);

	if (!plen)
		from = NULL;

	ret = -EINVAL;
	if (plen > 1024 * 1024 - 1)
		goto error;

	/* the appropriate instantiation authorisation key must have been
	 * assumed before calling this */
	ret = -EPERM;
	instkey = cred->request_key_auth;
	if (!instkey)
		goto error;

	rka = instkey->payload.data[0];
	if (rka->target_key->serial != id)
		goto error;

	/* pull the payload in if one was supplied */
	payload = NULL;

	if (from) {
		ret = -ENOMEM;
		payload = kmalloc(plen, GFP_KERNEL);
		if (!payload) {
			if (plen <= PAGE_SIZE)
				goto error;
			payload = vmalloc(plen);
			if (!payload)
				goto error;
		}

		ret = -EFAULT;
		if (!copy_from_iter_full(payload, plen, from))
			goto error2;
	}

	/* find the destination keyring amongst those belonging to the
	 * requesting task */
	ret = get_instantiation_keyring(ringid, rka, &dest_keyring);
	if (ret < 0)
		goto error2;

	/* instantiate the key and link it into a keyring */
	ret = key_instantiate_and_link(rka->target_key, payload, plen,
				       dest_keyring, instkey);

	key_put(dest_keyring);

	/* discard the assumed authority if it's just been disabled by
	 * instantiation of the key */
	if (ret == 0)
		keyctl_change_reqkey_auth(NULL);

error2:
	kvfree(payload);
error:
	return ret;
}