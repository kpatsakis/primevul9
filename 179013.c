
static HashTable *date_object_get_gc_timezone(zval *object, zval ***table, int *n TSRMLS_DC)
{

       *table = NULL;
       *n = 0;
       return zend_std_get_properties(object TSRMLS_CC);