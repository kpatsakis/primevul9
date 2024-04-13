struct nft_set *nf_tables_set_lookup_byid(const struct net *net,
					  const struct nlattr *nla)
{
	struct nft_trans *trans;
	u32 id = ntohl(nla_get_be32(nla));

	list_for_each_entry(trans, &net->nft.commit_list, list) {
		if (trans->msg_type == NFT_MSG_NEWSET &&
		    id == nft_trans_set_id(trans))
			return nft_trans_set(trans);
	}
	return ERR_PTR(-ENOENT);
}