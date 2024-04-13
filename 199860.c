static int snmp6_alloc_dev(struct inet6_dev *idev)
{
	if (snmp_mib_init((void __percpu **)idev->stats.ipv6,
			  sizeof(struct ipstats_mib),
			  __alignof__(struct ipstats_mib)) < 0)
		goto err_ip;
	idev->stats.icmpv6dev = kzalloc(sizeof(struct icmpv6_mib_device),
					GFP_KERNEL);
	if (!idev->stats.icmpv6dev)
		goto err_icmp;
	idev->stats.icmpv6msgdev = kzalloc(sizeof(struct icmpv6msg_mib_device),
					   GFP_KERNEL);
	if (!idev->stats.icmpv6msgdev)
		goto err_icmpmsg;

	return 0;

err_icmpmsg:
	kfree(idev->stats.icmpv6dev);
err_icmp:
	snmp_mib_free((void __percpu **)idev->stats.ipv6);
err_ip:
	return -ENOMEM;
}