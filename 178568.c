PHPAPI void var_push_dtor(php_unserialize_data_t *var_hashx, zval *rval)
{
	var_dtor_entries *var_hash;

	if (!var_hashx || !*var_hashx) {
		return;
	}

	var_hash = (*var_hashx)->last_dtor;
#if VAR_ENTRIES_DBG
	fprintf(stderr, "var_push_dtor(%ld): %d\n", var_hash?var_hash->used_slots:-1L, Z_TYPE_PP(rval));
#endif

	if (!var_hash || var_hash->used_slots == VAR_ENTRIES_MAX) {
		var_hash = emalloc(sizeof(var_dtor_entries));
		var_hash->used_slots = 0;
		var_hash->next = 0;

		if (!(*var_hashx)->first_dtor) {
			(*var_hashx)->first_dtor = var_hash;
		} else {
			((var_dtor_entries *) (*var_hashx)->last_dtor)->next = var_hash;
		}

		(*var_hashx)->last_dtor = var_hash;
	}

	ZVAL_COPY(&var_hash->data[var_hash->used_slots], rval);
	var_hash->used_slots++;
}