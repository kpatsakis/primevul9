int key_link(struct key *keyring, struct key *key)
{
	struct assoc_array_edit *edit;
	int ret;

	kenter("{%d,%d}", keyring->serial, atomic_read(&keyring->usage));

	key_check(keyring);
	key_check(key);

	if (test_bit(KEY_FLAG_TRUSTED_ONLY, &keyring->flags) &&
	    !test_bit(KEY_FLAG_TRUSTED, &key->flags))
		return -EPERM;

	ret = __key_link_begin(keyring, &key->index_key, &edit);
	if (ret == 0) {
		kdebug("begun {%d,%d}", keyring->serial, atomic_read(&keyring->usage));
		ret = __key_link_check_live_key(keyring, key);
		if (ret == 0)
			__key_link(key, &edit);
		__key_link_end(keyring, &key->index_key, edit);
	}

	kleave(" = %d {%d,%d}", ret, keyring->serial, atomic_read(&keyring->usage));
	return ret;
}