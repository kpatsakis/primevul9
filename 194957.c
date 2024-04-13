nft_rule_is_active(struct net *net, const struct nft_rule *rule)
{
	return (rule->genmask & (1 << net->nft.gencursor)) == 0;
}