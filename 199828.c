static int in6_dump_addrs(struct inet6_dev *idev, struct sk_buff *skb,
			  struct netlink_callback *cb, enum addr_type_t type,
			  int s_ip_idx, int *p_ip_idx)
{
	struct ifmcaddr6 *ifmca;
	struct ifacaddr6 *ifaca;
	int err = 1;
	int ip_idx = *p_ip_idx;

	read_lock_bh(&idev->lock);
	switch (type) {
	case UNICAST_ADDR: {
		struct inet6_ifaddr *ifa;

		/* unicast address incl. temp addr */
		list_for_each_entry(ifa, &idev->addr_list, if_list) {
			if (++ip_idx < s_ip_idx)
				continue;
			err = inet6_fill_ifaddr(skb, ifa,
						NETLINK_CB(cb->skb).portid,
						cb->nlh->nlmsg_seq,
						RTM_NEWADDR,
						NLM_F_MULTI);
			if (err <= 0)
				break;
			nl_dump_check_consistent(cb, nlmsg_hdr(skb));
		}
		break;
	}
	case MULTICAST_ADDR:
		/* multicast address */
		for (ifmca = idev->mc_list; ifmca;
		     ifmca = ifmca->next, ip_idx++) {
			if (ip_idx < s_ip_idx)
				continue;
			err = inet6_fill_ifmcaddr(skb, ifmca,
						  NETLINK_CB(cb->skb).portid,
						  cb->nlh->nlmsg_seq,
						  RTM_GETMULTICAST,
						  NLM_F_MULTI);
			if (err <= 0)
				break;
		}
		break;
	case ANYCAST_ADDR:
		/* anycast address */
		for (ifaca = idev->ac_list; ifaca;
		     ifaca = ifaca->aca_next, ip_idx++) {
			if (ip_idx < s_ip_idx)
				continue;
			err = inet6_fill_ifacaddr(skb, ifaca,
						  NETLINK_CB(cb->skb).portid,
						  cb->nlh->nlmsg_seq,
						  RTM_GETANYCAST,
						  NLM_F_MULTI);
			if (err <= 0)
				break;
		}
		break;
	default:
		break;
	}
	read_unlock_bh(&idev->lock);
	*p_ip_idx = ip_idx;
	return err;
}