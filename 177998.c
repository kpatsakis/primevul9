static void phar_manifest_copy_ctor(zval *zv) /* {{{ */
{
	phar_entry_info *info = emalloc(sizeof(phar_entry_info));
	memcpy(info, Z_PTR_P(zv), sizeof(phar_entry_info));
	Z_PTR_P(zv) = info;
}