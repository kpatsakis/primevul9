static struct nft_chain *nft_chain_lookup_byid(const struct net *net,
					       const struct nft_table *table,
					       const struct nlattr *nla)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	u32 id = ntohl(nla_get_be32(nla));
	struct nft_trans *trans;

	list_for_each_entry(trans, &nft_net->commit_list, list) {
		struct nft_chain *chain = trans->ctx.chain;

		if (trans->msg_type == NFT_MSG_NEWCHAIN &&
		    chain->table == table &&
		    id == nft_trans_chain_id(trans))
			return chain;
	}
	return ERR_PTR(-ENOENT);
}