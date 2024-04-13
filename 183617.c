static int ip_mc_check_igmp_msg(struct sk_buff *skb)
{
	switch (igmp_hdr(skb)->type) {
	case IGMP_HOST_LEAVE_MESSAGE:
	case IGMP_HOST_MEMBERSHIP_REPORT:
	case IGMPV2_HOST_MEMBERSHIP_REPORT:
		return 0;
	case IGMPV3_HOST_MEMBERSHIP_REPORT:
		return ip_mc_check_igmp_reportv3(skb);
	case IGMP_HOST_MEMBERSHIP_QUERY:
		return ip_mc_check_igmp_query(skb);
	default:
		return -ENOMSG;
	}
}