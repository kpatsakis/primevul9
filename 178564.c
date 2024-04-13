static inline int object_custom(UNSERIALIZE_PARAMETER, zend_class_entry *ce)
{
	zend_long datalen;

	datalen = parse_iv2((*p) + 2, p);

	(*p) += 2;

	if (datalen < 0 || (max - (*p)) <= datalen) {
		zend_error(E_WARNING, "Insufficient data for unserializing - %pd required, %pd present", datalen, (zend_long)(max - (*p)));
		return 0;
	}

	if (ce->unserialize == NULL) {
		zend_error(E_WARNING, "Class %s has no unserializer", ce->name->val);
		object_init_ex(rval, ce);
	} else if (ce->unserialize(rval, ce, (const unsigned char*)*p, datalen, (zend_unserialize_data *)var_hash) != SUCCESS) {
		return 0;
	}

	(*p) += datalen;

	return finish_nested_data(UNSERIALIZE_PASSTHRU);
}