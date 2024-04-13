void nft_unregister_set(struct nft_set_ops *ops)
{
	nfnl_lock(NFNL_SUBSYS_NFTABLES);
	list_del_rcu(&ops->list);
	nfnl_unlock(NFNL_SUBSYS_NFTABLES);
}