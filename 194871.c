static inline void nft_rule_clear(struct net *net, struct nft_rule *rule)
{
	rule->genmask = 0;
}