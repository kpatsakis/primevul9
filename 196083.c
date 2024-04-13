long keyctl_keyring_clear(key_serial_t ringid)
{
	key_ref_t keyring_ref;
	struct key *keyring;
	long ret;

	keyring_ref = lookup_user_key(ringid, KEY_LOOKUP_CREATE, KEY_NEED_WRITE);
	if (IS_ERR(keyring_ref)) {
		ret = PTR_ERR(keyring_ref);

		/* Root is permitted to invalidate certain special keyrings */
		if (capable(CAP_SYS_ADMIN)) {
			keyring_ref = lookup_user_key(ringid, 0, 0);
			if (IS_ERR(keyring_ref))
				goto error;
			if (test_bit(KEY_FLAG_ROOT_CAN_CLEAR,
				     &key_ref_to_ptr(keyring_ref)->flags))
				goto clear;
			goto error_put;
		}

		goto error;
	}

clear:
	keyring = key_ref_to_ptr(keyring_ref);
	if (test_bit(KEY_FLAG_KEEP, &keyring->flags))
		ret = -EPERM;
	else
		ret = keyring_clear(keyring);
error_put:
	key_ref_put(keyring_ref);
error:
	return ret;
}