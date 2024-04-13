static struct nft_set *nft_set_lookup_byid(const struct net *net,
					   const struct nft_table *table,
					   const struct nlattr *nla, u8 genmask)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	u32 id = ntohl(nla_get_be32(nla));
	struct nft_trans *trans;

	list_for_each_entry(trans, &nft_net->commit_list, list) {
		if (trans->msg_type == NFT_MSG_NEWSET) {
			struct nft_set *set = nft_trans_set(trans);

			if (id == nft_trans_set_id(trans) &&
			    set->table == table &&
			    nft_active_genmask(set, genmask))
				return set;
		}
	}
	return ERR_PTR(-ENOENT);
}