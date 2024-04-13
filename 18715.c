static struct xfrm_policy *__xfrm_policy_unlink(struct xfrm_policy *pol,
						int dir)
{
	struct net *net = xp_net(pol);

	if (list_empty(&pol->walk.all))
		return NULL;

	/* Socket policies are not hashed. */
	if (!hlist_unhashed(&pol->bydst)) {
		hlist_del_rcu(&pol->bydst);
		hlist_del_init(&pol->bydst_inexact_list);
		hlist_del(&pol->byidx);
	}

	list_del_init(&pol->walk.all);
	net->xfrm.policy_count[dir]--;

	return pol;
}