static int cgw_remove_job(struct sk_buff *skb, struct nlmsghdr *nlh,
			  struct netlink_ext_ack *extack)
{
	struct net *net = sock_net(skb->sk);
	struct cgw_job *gwj = NULL;
	struct hlist_node *nx;
	struct rtcanmsg *r;
	struct cf_mod mod;
	struct can_can_gw ccgw;
	u8 limhops = 0;
	int err = 0;

	if (!netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if (nlmsg_len(nlh) < sizeof(*r))
		return -EINVAL;

	r = nlmsg_data(nlh);
	if (r->can_family != AF_CAN)
		return -EPFNOSUPPORT;

	/* so far we only support CAN -> CAN routings */
	if (r->gwtype != CGW_TYPE_CAN_CAN)
		return -EINVAL;

	err = cgw_parse_attr(nlh, &mod, CGW_TYPE_CAN_CAN, &ccgw, &limhops);
	if (err < 0)
		return err;

	/* two interface indices both set to 0 => remove all entries */
	if (!ccgw.src_idx && !ccgw.dst_idx) {
		cgw_remove_all_jobs(net);
		return 0;
	}

	err = -EINVAL;

	ASSERT_RTNL();

	/* remove only the first matching entry */
	hlist_for_each_entry_safe(gwj, nx, &net->can.cgw_list, list) {

		if (gwj->flags != r->flags)
			continue;

		if (gwj->limit_hops != limhops)
			continue;

		/* we have a match when uid is enabled and identical */
		if (gwj->mod.uid || mod.uid) {
			if (gwj->mod.uid != mod.uid)
				continue;
		} else {
			/* no uid => check for identical modifications */
			if (memcmp(&gwj->mod, &mod, sizeof(mod)))
				continue;
		}

		/* if (r->gwtype == CGW_TYPE_CAN_CAN) - is made sure here */
		if (memcmp(&gwj->ccgw, &ccgw, sizeof(ccgw)))
			continue;

		hlist_del(&gwj->list);
		cgw_unregister_filter(net, gwj);
		kmem_cache_free(cgw_cache, gwj);
		err = 0;
		break;
	}

	return err;
}