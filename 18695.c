static u32 xfrm_pol_bin_obj(const void *data, u32 len, u32 seed)
{
	const struct xfrm_pol_inexact_bin *b = data;

	return xfrm_pol_bin_key(&b->k, 0, seed);
}