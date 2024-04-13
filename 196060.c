long keyctl_keyring_search(key_serial_t ringid,
			   const char __user *_type,
			   const char __user *_description,
			   key_serial_t destringid)
{
	struct key_type *ktype;
	key_ref_t keyring_ref, key_ref, dest_ref;
	char type[32], *description;
	long ret;

	/* pull the type and description into kernel space */
	ret = key_get_type_from_user(type, _type, sizeof(type));
	if (ret < 0)
		goto error;

	description = strndup_user(_description, KEY_MAX_DESC_SIZE);
	if (IS_ERR(description)) {
		ret = PTR_ERR(description);
		goto error;
	}

	/* get the keyring at which to begin the search */
	keyring_ref = lookup_user_key(ringid, 0, KEY_NEED_SEARCH);
	if (IS_ERR(keyring_ref)) {
		ret = PTR_ERR(keyring_ref);
		goto error2;
	}

	/* get the destination keyring if specified */
	dest_ref = NULL;
	if (destringid) {
		dest_ref = lookup_user_key(destringid, KEY_LOOKUP_CREATE,
					   KEY_NEED_WRITE);
		if (IS_ERR(dest_ref)) {
			ret = PTR_ERR(dest_ref);
			goto error3;
		}
	}

	/* find the key type */
	ktype = key_type_lookup(type);
	if (IS_ERR(ktype)) {
		ret = PTR_ERR(ktype);
		goto error4;
	}

	/* do the search */
	key_ref = keyring_search(keyring_ref, ktype, description);
	if (IS_ERR(key_ref)) {
		ret = PTR_ERR(key_ref);

		/* treat lack or presence of a negative key the same */
		if (ret == -EAGAIN)
			ret = -ENOKEY;
		goto error5;
	}

	/* link the resulting key to the destination keyring if we can */
	if (dest_ref) {
		ret = key_permission(key_ref, KEY_NEED_LINK);
		if (ret < 0)
			goto error6;

		ret = key_link(key_ref_to_ptr(dest_ref), key_ref_to_ptr(key_ref));
		if (ret < 0)
			goto error6;
	}

	ret = key_ref_to_ptr(key_ref)->serial;

error6:
	key_ref_put(key_ref);
error5:
	key_type_put(ktype);
error4:
	key_ref_put(dest_ref);
error3:
	key_ref_put(keyring_ref);
error2:
	kfree(description);
error:
	return ret;
}