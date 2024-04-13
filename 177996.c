static int phar_zip_changed_apply(zval *zv, void *arg) /* {{{ */
{
	return phar_zip_changed_apply_int(Z_PTR_P(zv), arg);
}