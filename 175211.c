static int ovs_dp_cmd_new(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr **a = info->attrs;
	struct vport_parms parms;
	struct sk_buff *reply;
	struct datapath *dp;
	struct vport *vport;
	struct ovs_net *ovs_net;
	int err, i;

	err = -EINVAL;
	if (!a[OVS_DP_ATTR_NAME] || !a[OVS_DP_ATTR_UPCALL_PID])
		goto err;

	ovs_lock();

	err = -ENOMEM;
	dp = kzalloc(sizeof(*dp), GFP_KERNEL);
	if (dp == NULL)
		goto err_unlock_ovs;

	ovs_dp_set_net(dp, hold_net(sock_net(skb->sk)));

	/* Allocate table. */
	err = ovs_flow_tbl_init(&dp->table);
	if (err)
		goto err_free_dp;

	dp->stats_percpu = alloc_percpu(struct dp_stats_percpu);
	if (!dp->stats_percpu) {
		err = -ENOMEM;
		goto err_destroy_table;
	}

	for_each_possible_cpu(i) {
		struct dp_stats_percpu *dpath_stats;
		dpath_stats = per_cpu_ptr(dp->stats_percpu, i);
		u64_stats_init(&dpath_stats->sync);
	}

	dp->ports = kmalloc(DP_VPORT_HASH_BUCKETS * sizeof(struct hlist_head),
			    GFP_KERNEL);
	if (!dp->ports) {
		err = -ENOMEM;
		goto err_destroy_percpu;
	}

	for (i = 0; i < DP_VPORT_HASH_BUCKETS; i++)
		INIT_HLIST_HEAD(&dp->ports[i]);

	/* Set up our datapath device. */
	parms.name = nla_data(a[OVS_DP_ATTR_NAME]);
	parms.type = OVS_VPORT_TYPE_INTERNAL;
	parms.options = NULL;
	parms.dp = dp;
	parms.port_no = OVSP_LOCAL;
	parms.upcall_portid = nla_get_u32(a[OVS_DP_ATTR_UPCALL_PID]);

	ovs_dp_change(dp, a);

	vport = new_vport(&parms);
	if (IS_ERR(vport)) {
		err = PTR_ERR(vport);
		if (err == -EBUSY)
			err = -EEXIST;

		if (err == -EEXIST) {
			/* An outdated user space instance that does not understand
			 * the concept of user_features has attempted to create a new
			 * datapath and is likely to reuse it. Drop all user features.
			 */
			if (info->genlhdr->version < OVS_DP_VER_FEATURES)
				ovs_dp_reset_user_features(skb, info);
		}

		goto err_destroy_ports_array;
	}

	reply = ovs_dp_cmd_build_info(dp, info, OVS_DP_CMD_NEW);
	err = PTR_ERR(reply);
	if (IS_ERR(reply))
		goto err_destroy_local_port;

	ovs_net = net_generic(ovs_dp_get_net(dp), ovs_net_id);
	list_add_tail_rcu(&dp->list_node, &ovs_net->dps);

	ovs_unlock();

	ovs_notify(&dp_datapath_genl_family, reply, info);
	return 0;

err_destroy_local_port:
	ovs_dp_detach_port(ovs_vport_ovsl(dp, OVSP_LOCAL));
err_destroy_ports_array:
	kfree(dp->ports);
err_destroy_percpu:
	free_percpu(dp->stats_percpu);
err_destroy_table:
	ovs_flow_tbl_destroy(&dp->table, false);
err_free_dp:
	release_net(ovs_dp_get_net(dp));
	kfree(dp);
err_unlock_ovs:
	ovs_unlock();
err:
	return err;
}