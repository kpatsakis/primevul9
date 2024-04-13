static int xfrm_pol_bin_cmp(struct rhashtable_compare_arg *arg,
			    const void *ptr)
{
	const struct xfrm_pol_inexact_key *key = arg->key;
	const struct xfrm_pol_inexact_bin *b = ptr;
	int ret;

	if (!net_eq(read_pnet(&b->k.net), read_pnet(&key->net)))
		return -1;

	ret = b->k.dir ^ key->dir;
	if (ret)
		return ret;

	ret = b->k.type ^ key->type;
	if (ret)
		return ret;

	ret = b->k.family ^ key->family;
	if (ret)
		return ret;

	return b->k.if_id ^ key->if_id;
}