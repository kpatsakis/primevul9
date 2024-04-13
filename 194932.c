int nft_register_set(struct nft_set_ops *ops)
{
	nfnl_lock(NFNL_SUBSYS_NFTABLES);
	list_add_tail_rcu(&ops->list, &nf_tables_set_ops);
	nfnl_unlock(NFNL_SUBSYS_NFTABLES);
	return 0;
}