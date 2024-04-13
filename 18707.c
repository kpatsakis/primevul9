xfrm_policy_inexact_lookup_rcu(struct net *net, u8 type, u16 family,
			       u8 dir, u32 if_id)
{
	struct xfrm_pol_inexact_key k = {
		.family = family,
		.type = type,
		.dir = dir,
		.if_id = if_id,
	};

	write_pnet(&k.net, net);

	return rhashtable_lookup(&xfrm_policy_inexact_table, &k,
				 xfrm_pol_inexact_params);
}