static bool keyring_gc_select_iterator(void *object, void *iterator_data)
{
	struct key *key = keyring_ptr_to_key(object);
	time_t *limit = iterator_data;

	if (key_is_dead(key, *limit))
		return false;
	key_get(key);
	return true;
}