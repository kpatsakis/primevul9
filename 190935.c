 */
static inline zval *add_next_index_object(zval *arg, zval *tmp)
{
	HashTable *symtable;

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		symtable = Z_OBJPROP_P(arg);
	} else {
		symtable = Z_ARRVAL_P(arg);
	}

	return zend_hash_next_index_insert(symtable, tmp);