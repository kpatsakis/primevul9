static int __init netlink_proto_init(void)
{
	int i;
	int err = proto_register(&netlink_proto, 0);

	if (err != 0)
		goto out;

	BUILD_BUG_ON(sizeof(struct netlink_skb_parms) > FIELD_SIZEOF(struct sk_buff, cb));

	nl_table = kcalloc(MAX_LINKS, sizeof(*nl_table), GFP_KERNEL);
	if (!nl_table)
		goto panic;

	for (i = 0; i < MAX_LINKS; i++) {
		if (rhashtable_init(&nl_table[i].hash,
				    &netlink_rhashtable_params) < 0) {
			while (--i > 0)
				rhashtable_destroy(&nl_table[i].hash);
			kfree(nl_table);
			goto panic;
		}
	}

	INIT_LIST_HEAD(&netlink_tap_all);

	netlink_add_usersock_entry();

	sock_register(&netlink_family_ops);
	register_pernet_subsys(&netlink_net_ops);
	/* The netlink device handler may be needed early. */
	rtnetlink_init();
out:
	return err;
panic:
	panic("netlink_init: Cannot allocate nl_table\n");
}