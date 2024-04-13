void ovs_dp_process_received_packet(struct vport *p, struct sk_buff *skb)
{
	struct datapath *dp = p->dp;
	struct sw_flow *flow;
	struct dp_stats_percpu *stats;
	struct sw_flow_key key;
	u64 *stats_counter;
	u32 n_mask_hit;
	int error;

	stats = this_cpu_ptr(dp->stats_percpu);

	/* Extract flow from 'skb' into 'key'. */
	error = ovs_flow_extract(skb, p->port_no, &key);
	if (unlikely(error)) {
		kfree_skb(skb);
		return;
	}

	/* Look up flow. */
	flow = ovs_flow_tbl_lookup_stats(&dp->table, &key, &n_mask_hit);
	if (unlikely(!flow)) {
		struct dp_upcall_info upcall;

		upcall.cmd = OVS_PACKET_CMD_MISS;
		upcall.key = &key;
		upcall.userdata = NULL;
		upcall.portid = p->upcall_portid;
		ovs_dp_upcall(dp, skb, &upcall);
		consume_skb(skb);
		stats_counter = &stats->n_missed;
		goto out;
	}

	OVS_CB(skb)->flow = flow;
	OVS_CB(skb)->pkt_key = &key;

	ovs_flow_stats_update(OVS_CB(skb)->flow, skb);
	ovs_execute_actions(dp, skb);
	stats_counter = &stats->n_hit;

out:
	/* Update datapath statistics. */
	u64_stats_update_begin(&stats->sync);
	(*stats_counter)++;
	stats->n_mask_hit += n_mask_hit;
	u64_stats_update_end(&stats->sync);
}