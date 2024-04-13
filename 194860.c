int nft_register_afinfo(struct net *net, struct nft_af_info *afi)
{
	INIT_LIST_HEAD(&afi->tables);
	nfnl_lock(NFNL_SUBSYS_NFTABLES);
	list_add_tail_rcu(&afi->list, &net->nft.af_info);
	nfnl_unlock(NFNL_SUBSYS_NFTABLES);
	return 0;
}