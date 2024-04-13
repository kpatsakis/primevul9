nft_rule_activate_next(struct net *net, struct nft_rule *rule)
{
	/* Now inactive, will be active in the future */
	rule->genmask = (1 << net->nft.gencursor);
}