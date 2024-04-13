void sc_file_dup(sc_file_t **dest, const sc_file_t *src)
{
	sc_file_t *newf;
	const sc_acl_entry_t *e;
	unsigned int op;

	*dest = NULL;
	if (!sc_file_valid(src))
		return;
	newf = sc_file_new();
	if (newf == NULL)
		return;
	*dest = newf;

	memcpy(&newf->path, &src->path, sizeof(struct sc_path));
	memcpy(&newf->name, &src->name, sizeof(src->name));
	newf->namelen = src->namelen;
	newf->type    = src->type;
	newf->shareable    = src->shareable;
	newf->ef_structure = src->ef_structure;
	newf->size    = src->size;
	newf->id      = src->id;
	newf->status  = src->status;
	for (op = 0; op < SC_MAX_AC_OPS; op++) {
		newf->acl[op] = NULL;
		e = sc_file_get_acl_entry(src, op);
		if (e != NULL) {
			if (sc_file_add_acl_entry(newf, op, e->method, e->key_ref) < 0)
				goto err;
		}
	}
	newf->record_length = src->record_length;
	newf->record_count  = src->record_count;

	if (sc_file_set_sec_attr(newf, src->sec_attr, src->sec_attr_len) < 0)
		goto err;
	if (sc_file_set_prop_attr(newf, src->prop_attr, src->prop_attr_len) < 0)
		goto err;
	if (sc_file_set_type_attr(newf, src->type_attr, src->type_attr_len) < 0)
		goto err;
	if (sc_file_set_content(newf, src->encoded_content, src->encoded_content_len) < 0)
		goto err;
	return;
err:
	sc_file_free(newf);
	*dest = NULL;
}