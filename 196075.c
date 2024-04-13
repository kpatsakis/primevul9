long keyctl_keyring_unlink(key_serial_t id, key_serial_t ringid)
{
	key_ref_t keyring_ref, key_ref;
	struct key *keyring, *key;
	long ret;

	keyring_ref = lookup_user_key(ringid, 0, KEY_NEED_WRITE);
	if (IS_ERR(keyring_ref)) {
		ret = PTR_ERR(keyring_ref);
		goto error;
	}

	key_ref = lookup_user_key(id, KEY_LOOKUP_FOR_UNLINK, 0);
	if (IS_ERR(key_ref)) {
		ret = PTR_ERR(key_ref);
		goto error2;
	}

	keyring = key_ref_to_ptr(keyring_ref);
	key = key_ref_to_ptr(key_ref);
	if (test_bit(KEY_FLAG_KEEP, &keyring->flags) &&
	    test_bit(KEY_FLAG_KEEP, &key->flags))
		ret = -EPERM;
	else
		ret = key_unlink(keyring, key);

	key_ref_put(key_ref);
error2:
	key_ref_put(keyring_ref);
error:
	return ret;
}