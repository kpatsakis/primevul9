static void snmp6_free_dev(struct inet6_dev *idev)
{
	kfree(idev->stats.icmpv6msgdev);
	kfree(idev->stats.icmpv6dev);
	snmp_mib_free((void __percpu **)idev->stats.ipv6);
}