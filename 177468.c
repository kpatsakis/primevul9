create_footnote_ref(struct footnote_list *list, const uint8_t *name, size_t name_size)
{
	struct footnote_ref *ref = calloc(1, sizeof(struct footnote_ref));
	if (!ref)
		return NULL;

	ref->id = hash_link_ref(name, name_size);

	return ref;
}