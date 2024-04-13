ZEND_API int zend_next_free_module(void) /* {{{ */
{
	return zend_hash_num_elements(&module_registry) + 1;
}