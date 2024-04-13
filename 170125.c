void keyring_gc(struct key *keyring, time_t limit)
{
	int result;

	kenter("%x{%s}", keyring->serial, keyring->description ?: "");

	if (keyring->flags & ((1 << KEY_FLAG_INVALIDATED) |
			      (1 << KEY_FLAG_REVOKED)))
		goto dont_gc;

	/* scan the keyring looking for dead keys */
	rcu_read_lock();
	result = assoc_array_iterate(&keyring->keys,
				     keyring_gc_check_iterator, &limit);
	rcu_read_unlock();
	if (result == true)
		goto do_gc;

dont_gc:
	kleave(" [no gc]");
	return;

do_gc:
	down_write(&keyring->sem);
	assoc_array_gc(&keyring->keys, &keyring_assoc_array_ops,
		       keyring_gc_select_iterator, &limit);
	up_write(&keyring->sem);
	kleave(" [gc]");
}