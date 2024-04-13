static int phar_tar_writeheaders(zval *zv, void *argument) /* {{{ */
{
	return phar_tar_writeheaders_int(Z_PTR_P(zv), argument);
}