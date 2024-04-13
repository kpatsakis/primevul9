void sc_mem_clear(void *ptr, size_t len)
{
	if (len > 0)   {
#ifdef ENABLE_OPENSSL
		OPENSSL_cleanse(ptr, len);
#else
		memset(ptr, 0, len);
#endif
	}
}