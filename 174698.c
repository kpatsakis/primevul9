_wrap_cmac256_set_key(void *ctx, size_t len, const uint8_t * key)
{
	if (unlikely(len != 32))
		abort();
	cmac_aes256_set_key(ctx, key);
}