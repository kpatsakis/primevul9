static void snmp6_fill_stats(u64 *stats, struct inet6_dev *idev, int attrtype,
			     int bytes)
{
	switch (attrtype) {
	case IFLA_INET6_STATS:
		__snmp6_fill_stats64(stats, (void __percpu **)idev->stats.ipv6,
				     IPSTATS_MIB_MAX, bytes, offsetof(struct ipstats_mib, syncp));
		break;
	case IFLA_INET6_ICMP6STATS:
		__snmp6_fill_statsdev(stats, idev->stats.icmpv6dev->mibs, ICMP6_MIB_MAX, bytes);
		break;
	}
}