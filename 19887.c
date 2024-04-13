static int nsh_key_to_nlattr(const struct ovs_key_nsh *nsh, bool is_mask,
			     struct sk_buff *skb)
{
	struct nlattr *start;

	start = nla_nest_start_noflag(skb, OVS_KEY_ATTR_NSH);
	if (!start)
		return -EMSGSIZE;

	if (nla_put(skb, OVS_NSH_KEY_ATTR_BASE, sizeof(nsh->base), &nsh->base))
		goto nla_put_failure;

	if (is_mask || nsh->base.mdtype == NSH_M_TYPE1) {
		if (nla_put(skb, OVS_NSH_KEY_ATTR_MD1,
			    sizeof(nsh->context), nsh->context))
			goto nla_put_failure;
	}

	/* Don't support MD type 2 yet */

	nla_nest_end(skb, start);

	return 0;

nla_put_failure:
	return -EMSGSIZE;
}