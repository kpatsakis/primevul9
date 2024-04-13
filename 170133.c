int key_unlink(struct key *keyring, struct key *key)
{
	struct assoc_array_edit *edit;
	int ret;

	key_check(keyring);
	key_check(key);

	if (keyring->type != &key_type_keyring)
		return -ENOTDIR;

	down_write(&keyring->sem);

	edit = assoc_array_delete(&keyring->keys, &keyring_assoc_array_ops,
				  &key->index_key);
	if (IS_ERR(edit)) {
		ret = PTR_ERR(edit);
		goto error;
	}
	ret = -ENOENT;
	if (edit == NULL)
		goto error;

	assoc_array_apply_edit(edit);
	key_payload_reserve(keyring, keyring->datalen - KEYQUOTA_LINK_BYTES);
	ret = 0;

error:
	up_write(&keyring->sem);
	return ret;
}