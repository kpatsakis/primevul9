static void scrub_parity_get(struct scrub_parity *sparity)
{
	refcount_inc(&sparity->refs);
}