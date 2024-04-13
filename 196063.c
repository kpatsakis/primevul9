long keyctl_setperm_key(key_serial_t id, key_perm_t perm)
{
	struct key *key;
	key_ref_t key_ref;
	long ret;

	ret = -EINVAL;
	if (perm & ~(KEY_POS_ALL | KEY_USR_ALL | KEY_GRP_ALL | KEY_OTH_ALL))
		goto error;

	key_ref = lookup_user_key(id, KEY_LOOKUP_CREATE | KEY_LOOKUP_PARTIAL,
				  KEY_NEED_SETATTR);
	if (IS_ERR(key_ref)) {
		ret = PTR_ERR(key_ref);
		goto error;
	}

	key = key_ref_to_ptr(key_ref);

	/* make the changes with the locks held to prevent chown/chmod races */
	ret = -EACCES;
	down_write(&key->sem);

	/* if we're not the sysadmin, we can only change a key that we own */
	if (capable(CAP_SYS_ADMIN) || uid_eq(key->uid, current_fsuid())) {
		key->perm = perm;
		ret = 0;
	}

	up_write(&key->sem);
	key_put(key);
error:
	return ret;
}