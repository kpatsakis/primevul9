bool gfp_pfmemalloc_allowed(gfp_t gfp_mask)
{
	return !!__gfp_pfmemalloc_flags(gfp_mask);
}