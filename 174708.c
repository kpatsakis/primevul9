_wrap_umac96_set_key(void *ctx, size_t len, const uint8_t * key)
{
	if (unlikely(len != 16))
		abort();
	umac96_set_key(ctx, key);
}