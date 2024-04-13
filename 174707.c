_wrap_gost28147_imit_set_key_tc26z(void *ctx, size_t len, const uint8_t * key)
{
	gost28147_imit_set_param(ctx, &gost28147_param_TC26_Z);
	gost28147_imit_set_key(ctx, len, key);
}