static void igmp_ifc_event(struct in_device *in_dev)
{
	struct net *net = dev_net(in_dev->dev);
	if (IGMP_V1_SEEN(in_dev) || IGMP_V2_SEEN(in_dev))
		return;
	in_dev->mr_ifc_count = in_dev->mr_qrv ?: net->ipv4.sysctl_igmp_qrv;
	igmp_ifc_start_timer(in_dev, 1);
}