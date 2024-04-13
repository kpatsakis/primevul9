long keyctl_describe_key(key_serial_t keyid,
			 char __user *buffer,
			 size_t buflen)
{
	struct key *key, *instkey;
	key_ref_t key_ref;
	char *infobuf;
	long ret;
	int desclen, infolen;

	key_ref = lookup_user_key(keyid, KEY_LOOKUP_PARTIAL, KEY_NEED_VIEW);
	if (IS_ERR(key_ref)) {
		/* viewing a key under construction is permitted if we have the
		 * authorisation token handy */
		if (PTR_ERR(key_ref) == -EACCES) {
			instkey = key_get_instantiation_authkey(keyid);
			if (!IS_ERR(instkey)) {
				key_put(instkey);
				key_ref = lookup_user_key(keyid,
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
	desclen = strlen(key->description);

	/* calculate how much information we're going to return */
	ret = -ENOMEM;
	infobuf = kasprintf(GFP_KERNEL,
			    "%s;%d;%d;%08x;",
			    key->type->name,
			    from_kuid_munged(current_user_ns(), key->uid),
			    from_kgid_munged(current_user_ns(), key->gid),
			    key->perm);
	if (!infobuf)
		goto error2;
	infolen = strlen(infobuf);
	ret = infolen + desclen + 1;

	/* consider returning the data */
	if (buffer && buflen >= ret) {
		if (copy_to_user(buffer, infobuf, infolen) != 0 ||
		    copy_to_user(buffer + infolen, key->description,
				 desclen + 1) != 0)
			ret = -EFAULT;
	}

	kfree(infobuf);
error2:
	key_ref_put(key_ref);
error:
	return ret;
}