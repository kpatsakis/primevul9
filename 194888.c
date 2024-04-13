nft_rule_is_active_next(struct net *net, const struct nft_rule *rule)
{
	return (rule->genmask & (1 << gencursor_next(net))) == 0;
}