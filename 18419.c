static void __mix_pool_bytes(struct entropy_store *r, const void *in,
			     int nbytes)
{
	trace_mix_pool_bytes_nolock(r->name, nbytes, _RET_IP_);
	_mix_pool_bytes(r, in, nbytes);
}