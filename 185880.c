static inline enum rt6_nud_state rt6_check_neigh(struct rt6_info *rt)
{
	struct neighbour *neigh;
	enum rt6_nud_state ret = RT6_NUD_FAIL_HARD;

	if (rt->rt6i_flags & RTF_NONEXTHOP ||
	    !(rt->rt6i_flags & RTF_GATEWAY))
		return RT6_NUD_SUCCEED;

	rcu_read_lock_bh();
	neigh = __ipv6_neigh_lookup_noref(rt->dst.dev, &rt->rt6i_gateway);
	if (neigh) {
		read_lock(&neigh->lock);
		if (neigh->nud_state & NUD_VALID)
			ret = RT6_NUD_SUCCEED;
#ifdef CONFIG_IPV6_ROUTER_PREF
		else if (!(neigh->nud_state & NUD_FAILED))
			ret = RT6_NUD_SUCCEED;
		else
			ret = RT6_NUD_FAIL_PROBE;
#endif
		read_unlock(&neigh->lock);
	} else {
		ret = IS_ENABLED(CONFIG_IPV6_ROUTER_PREF) ?
		      RT6_NUD_SUCCEED : RT6_NUD_FAIL_DO_RR;
	}
	rcu_read_unlock_bh();

	return ret;
}