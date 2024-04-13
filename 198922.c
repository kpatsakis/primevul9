PHP_RSHUTDOWN_FUNCTION(mb_regex)
{
	MBREX(current_mbctype) = MBREX(default_mbctype);

	if (MBREX(search_str) != NULL) {
		zval_ptr_dtor(&MBREX(search_str));
		MBREX(search_str) = (zval *)NULL;
	}
	MBREX(search_pos) = 0;

	if (MBREX(search_regs) != NULL) {
		onig_region_free(MBREX(search_regs), 1);
		MBREX(search_regs) = (OnigRegion *)NULL;
	}
	zend_hash_clean(&MBREX(ht_rc));

	return SUCCESS;
}