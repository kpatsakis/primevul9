long keyctl_set_timeout(key_serial_t id, unsigned timeout)
{
	struct key *key, *instkey;
	key_ref_t key_ref;
	long ret;

	key_ref = lookup_user_key(id, KEY_LOOKUP_CREATE | KEY_LOOKUP_PARTIAL,
				  KEY_NEED_SETATTR);
	if (IS_ERR(key_ref)) {
		/* setting the timeout on a key under construction is permitted
		 * if we have the authorisation token handy */
		if (PTR_ERR(key_ref) == -EACCES) {
			instkey = key_get_instantiation_authkey(id);
			if (!IS_ERR(instkey)) {
				key_put(instkey);
				key_ref = lookup_user_key(id,
							  KEY_LOOKUP_PARTIAL,
							  0);
				if (!IS_ERR(key_ref))
					goto okay;
			}
		}

		ret = PTR_ERR(key_ref);
		goto error;
	}

okay:
	key = key_ref_to_ptr(key_ref);
	ret = 0;
	if (test_bit(KEY_FLAG_KEEP, &key->flags))
		ret = -EPERM;
	else
		key_set_timeout(key, timeout);
	key_put(key);

error:
	return ret;
}