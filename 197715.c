static int cgw_create_job(struct sk_buff *skb,  struct nlmsghdr *nlh,
			  struct netlink_ext_ack *extack)
{
	struct net *net = sock_net(skb->sk);
	struct rtcanmsg *r;
	struct cgw_job *gwj;
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

	if (mod.uid) {

		ASSERT_RTNL();

		/* check for updating an existing job with identical uid */
		hlist_for_each_entry(gwj, &net->can.cgw_list, list) {

			if (gwj->mod.uid != mod.uid)
				continue;

			/* interfaces & filters must be identical */
			if (memcmp(&gwj->ccgw, &ccgw, sizeof(ccgw)))
				return -EINVAL;

			/* update modifications with disabled softirq & quit */
			local_bh_disable();
			memcpy(&gwj->mod, &mod, sizeof(mod));
			local_bh_enable();
			return 0;
		}
	}

	/* ifindex == 0 is not allowed for job creation */
	if (!ccgw.src_idx || !ccgw.dst_idx)
		return -ENODEV;

	gwj = kmem_cache_alloc(cgw_cache, GFP_KERNEL);
	if (!gwj)
		return -ENOMEM;

	gwj->handled_frames = 0;
	gwj->dropped_frames = 0;
	gwj->deleted_frames = 0;
	gwj->flags = r->flags;
	gwj->gwtype = r->gwtype;
	gwj->limit_hops = limhops;

	/* insert already parsed information */
	memcpy(&gwj->mod, &mod, sizeof(mod));
	memcpy(&gwj->ccgw, &ccgw, sizeof(ccgw));

	err = -ENODEV;

	gwj->src.dev = __dev_get_by_index(net, gwj->ccgw.src_idx);

	if (!gwj->src.dev)
		goto out;

	if (gwj->src.dev->type != ARPHRD_CAN)
		goto out;

	gwj->dst.dev = __dev_get_by_index(net, gwj->ccgw.dst_idx);

	if (!gwj->dst.dev)
		goto out;

	if (gwj->dst.dev->type != ARPHRD_CAN)
		goto out;

	ASSERT_RTNL();

	err = cgw_register_filter(net, gwj);
	if (!err)
		hlist_add_head_rcu(&gwj->list, &net->can.cgw_list);
out:
	if (err)
		kmem_cache_free(cgw_cache, gwj);

	return err;
}