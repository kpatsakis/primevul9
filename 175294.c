int ovs_dp_upcall(struct datapath *dp, struct sk_buff *skb,
		  const struct dp_upcall_info *upcall_info)
{
	struct dp_stats_percpu *stats;
	int err;

	if (upcall_info->portid == 0) {
		err = -ENOTCONN;
		goto err;
	}

	if (!skb_is_gso(skb))
		err = queue_userspace_packet(dp, skb, upcall_info);
	else
		err = queue_gso_packets(dp, skb, upcall_info);
	if (err)
		goto err;

	return 0;

err:
	stats = this_cpu_ptr(dp->stats_percpu);

	u64_stats_update_begin(&stats->sync);
	stats->n_lost++;
	u64_stats_update_end(&stats->sync);

	return err;
}