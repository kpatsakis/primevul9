/* {{{ module_name_cmp */
static int module_name_cmp(const void *a, const void *b TSRMLS_DC)
{
	Bucket *f = *((Bucket **) a);
	Bucket *s = *((Bucket **) b);

	return strcasecmp(((zend_module_entry *)f->pData)->name,
				  ((zend_module_entry *)s->pData)->name);