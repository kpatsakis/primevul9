_crypt_extended(const char *key, const char *setting)
{
	static int initialized = 0;
	static struct php_crypt_extended_data data;

	if (!initialized) {
		_crypt_extended_init();
		initialized = 1;
		data.initialized = 0;
	}
	return _crypt_extended_r(key, setting, &data);
}