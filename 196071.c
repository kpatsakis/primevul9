SYSCALL_DEFINE4(request_key, const char __user *, _type,
		const char __user *, _description,
		const char __user *, _callout_info,
		key_serial_t, destringid)
{
	struct key_type *ktype;
	struct key *key;
	key_ref_t dest_ref;
	size_t callout_len;
	char type[32], *description, *callout_info;
	long ret;

	/* pull the type into kernel space */
	ret = key_get_type_from_user(type, _type, sizeof(type));
	if (ret < 0)
		goto error;

	/* pull the description into kernel space */
	description = strndup_user(_description, KEY_MAX_DESC_SIZE);
	if (IS_ERR(description)) {
		ret = PTR_ERR(description);
		goto error;
	}

	/* pull the callout info into kernel space */
	callout_info = NULL;
	callout_len = 0;
	if (_callout_info) {
		callout_info = strndup_user(_callout_info, PAGE_SIZE);
		if (IS_ERR(callout_info)) {
			ret = PTR_ERR(callout_info);
			goto error2;
		}
		callout_len = strlen(callout_info);
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
	key = request_key_and_link(ktype, description, callout_info,
				   callout_len, NULL, key_ref_to_ptr(dest_ref),
				   KEY_ALLOC_IN_QUOTA);
	if (IS_ERR(key)) {
		ret = PTR_ERR(key);
		goto error5;
	}

	/* wait for the key to finish being constructed */
	ret = wait_for_key_construction(key, 1);
	if (ret < 0)
		goto error6;

	ret = key->serial;

error6:
 	key_put(key);
error5:
	key_type_put(ktype);
error4:
	key_ref_put(dest_ref);
error3:
	kfree(callout_info);
error2:
	kfree(description);
error:
	return ret;
}