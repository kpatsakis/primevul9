xfrm_get_saddr(struct net *net, int oif, xfrm_address_t *local,
	       xfrm_address_t *remote, unsigned short family, u32 mark)
{
	int err;
	const struct xfrm_policy_afinfo *afinfo = xfrm_policy_get_afinfo(family);

	if (unlikely(afinfo == NULL))
		return -EINVAL;
	err = afinfo->get_saddr(net, oif, local, remote, mark);
	rcu_read_unlock();
	return err;
}