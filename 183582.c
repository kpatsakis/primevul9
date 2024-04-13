static void ip_mc_reset(struct in_device *in_dev)
{
	struct net *net = dev_net(in_dev->dev);

	in_dev->mr_qi = IGMP_QUERY_INTERVAL;
	in_dev->mr_qri = IGMP_QUERY_RESPONSE_INTERVAL;
	in_dev->mr_qrv = net->ipv4.sysctl_igmp_qrv;
}