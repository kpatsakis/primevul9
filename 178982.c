static int atalk_seq_route_show(struct seq_file *seq, void *v)
{
	struct atalk_route *rt;

	if (v == SEQ_START_TOKEN) {
		seq_puts(seq, "Target        Router  Flags Dev\n");
		goto out;
	}

	if (atrtr_default.dev) {
		rt = &atrtr_default;
		seq_printf(seq, "Default     %04X:%02X  %-4d  %s\n",
			       ntohs(rt->gateway.s_net), rt->gateway.s_node,
			       rt->flags, rt->dev->name);
	}

	rt = v;
	seq_printf(seq, "%04X:%02X     %04X:%02X  %-4d  %s\n",
		   ntohs(rt->target.s_net), rt->target.s_node,
		   ntohs(rt->gateway.s_net), rt->gateway.s_node,
		   rt->flags, rt->dev->name);
out:
	return 0;
}