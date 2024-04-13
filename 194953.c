void nft_unregister_afinfo(struct nft_af_info *afi)
{
	nfnl_lock(NFNL_SUBSYS_NFTABLES);
	list_del_rcu(&afi->list);
	nfnl_unlock(NFNL_SUBSYS_NFTABLES);
}