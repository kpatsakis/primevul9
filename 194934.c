nft_rule_deactivate_next(struct net *net, struct nft_rule *rule)
{
	rule->genmask = (1 << gencursor_next(net));
}