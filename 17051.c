static struct nft_rule *nft_rule_lookup_byid(const struct net *net,
					     const struct nft_chain *chain,
					     const struct nlattr *nla)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	u32 id = ntohl(nla_get_be32(nla));
	struct nft_trans *trans;

	list_for_each_entry(trans, &nft_net->commit_list, list) {
		struct nft_rule *rule = nft_trans_rule(trans);

		if (trans->msg_type == NFT_MSG_NEWRULE &&
		    trans->ctx.chain == chain &&
		    id == nft_trans_rule_id(trans))
			return rule;
	}
	return ERR_PTR(-ENOENT);
}