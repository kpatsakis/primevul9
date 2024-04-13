int ovs_nla_put_key(const struct sw_flow_key *swkey,
		    const struct sw_flow_key *output, int attr, bool is_mask,
		    struct sk_buff *skb)
{
	int err;
	struct nlattr *nla;

	nla = nla_nest_start_noflag(skb, attr);
	if (!nla)
		return -EMSGSIZE;
	err = __ovs_nla_put_key(swkey, output, is_mask, skb);
	if (err)
		return err;
	nla_nest_end(skb, nla);

	return 0;
}