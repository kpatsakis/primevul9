ZEND_API const char* zend_find_alias_name(zend_class_entry *ce, const char *name, zend_uint len) /* {{{ */
{
	zend_trait_alias *alias, **alias_ptr;

	if ((alias_ptr = ce->trait_aliases)) {
		alias = *alias_ptr;
		while (alias) {
			if (alias->alias_len == len &&
				!strncasecmp(name, alias->alias, alias->alias_len)) {
				return alias->alias;
			}
			alias_ptr++;
			alias = *alias_ptr;
		}
	}

	return name;
}