show_other_keys(struct hostkeys *hostkeys, Key *key)
{
	int type[] = {
		KEY_RSA1,
		KEY_RSA,
		KEY_DSA,
		KEY_ECDSA,
		KEY_ED25519,
		-1
	};
	int i, ret = 0;
	char *fp, *ra;
	const struct hostkey_entry *found;

	for (i = 0; type[i] != -1; i++) {
		if (type[i] == key->type)
			continue;
		if (!lookup_key_in_hostkeys_by_type(hostkeys, type[i], &found))
			continue;
		fp = key_fingerprint(found->key, SSH_FP_MD5, SSH_FP_HEX);
		ra = key_fingerprint(found->key, SSH_FP_MD5, SSH_FP_RANDOMART);
		logit("WARNING: %s key found for host %s\n"
		    "in %s:%lu\n"
		    "%s key fingerprint %s.",
		    key_type(found->key),
		    found->host, found->file, found->line,
		    key_type(found->key), fp);
		if (options.visual_host_key)
			logit("%s", ra);
		free(ra);
		free(fp);
		ret = 1;
	}
	return ret;
}