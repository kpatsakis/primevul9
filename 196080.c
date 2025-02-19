long keyctl_join_session_keyring(const char __user *_name)
{
	char *name;
	long ret;

	/* fetch the name from userspace */
	name = NULL;
	if (_name) {
		name = strndup_user(_name, KEY_MAX_DESC_SIZE);
		if (IS_ERR(name)) {
			ret = PTR_ERR(name);
			goto error;
		}

		ret = -EPERM;
		if (name[0] == '.')
			goto error_name;
	}

	/* join the session */
	ret = join_session_keyring(name);
error_name:
	kfree(name);
error:
	return ret;
}