static int nf_tables_init_net(struct net *net)
{
	INIT_LIST_HEAD(&net->nft.af_info);
	INIT_LIST_HEAD(&net->nft.commit_list);
	net->nft.base_seq = 1;
	return 0;
}