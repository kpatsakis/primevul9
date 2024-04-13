_wrap_umac128_set_key(void *ctx, size_t len, const uint8_t * key)
{
	if (unlikely(len != 16))
		abort();
	umac128_set_key(ctx, key);
}