MBSTRING_API int php_unicode_is_prop1(unsigned long code, int prop)
{
	return prop_lookup(code, prop);
}