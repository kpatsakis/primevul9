long keyctl_get_keyring_ID(key_serial_t id, int create)
{
	key_ref_t key_ref;
	unsigned long lflags;
	long ret;

	lflags = create ? KEY_LOOKUP_CREATE : 0;
	key_ref = lookup_user_key(id, lflags, KEY_NEED_SEARCH);
	if (IS_ERR(key_ref)) {
		ret = PTR_ERR(key_ref);
		goto error;
	}

	ret = key_ref_to_ptr(key_ref)->serial;
	key_ref_put(key_ref);
error:
	return ret;
}