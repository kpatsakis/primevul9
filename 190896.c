 */
static zval *add_assoc_object(zval *arg, char *key, zval *tmp)
{
	HashTable *symtable;

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		symtable = Z_OBJPROP_P(arg);
	} else {
		symtable = Z_ARRVAL_P(arg);
	}
	return zend_hash_str_update(symtable, key, strlen(key), tmp);