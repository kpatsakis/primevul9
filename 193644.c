static void __net_exit crypto_netlink_exit(struct net *net)
{
	netlink_kernel_release(net->crypto_nlsk);
	net->crypto_nlsk = NULL;
}