static u32 xfrm_pol_bin_key(const void *data, u32 len, u32 seed)
{
	const struct xfrm_pol_inexact_key *k = data;
	u32 a = k->type << 24 | k->dir << 16 | k->family;

	return jhash_3words(a, k->if_id, net_hash_mix(read_pnet(&k->net)),
			    seed);
}