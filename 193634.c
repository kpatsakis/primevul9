static int __net_init crypto_netlink_init(struct net *net)
{
	struct netlink_kernel_cfg cfg = {
		.input	= crypto_netlink_rcv,
	};

	net->crypto_nlsk = netlink_kernel_create(net, NETLINK_CRYPTO, &cfg);
	return net->crypto_nlsk == NULL ? -ENOMEM : 0;
}