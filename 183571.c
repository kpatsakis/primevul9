static int igmp_mc_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN)
		seq_puts(seq,
			 "Idx\tDevice    : Count Querier\tGroup    Users Timer\tReporter\n");
	else {
		struct ip_mc_list *im = (struct ip_mc_list *)v;
		struct igmp_mc_iter_state *state = igmp_mc_seq_private(seq);
		char   *querier;
		long delta;

#ifdef CONFIG_IP_MULTICAST
		querier = IGMP_V1_SEEN(state->in_dev) ? "V1" :
			  IGMP_V2_SEEN(state->in_dev) ? "V2" :
			  "V3";
#else
		querier = "NONE";
#endif

		if (rcu_access_pointer(state->in_dev->mc_list) == im) {
			seq_printf(seq, "%d\t%-10s: %5d %7s\n",
				   state->dev->ifindex, state->dev->name, state->in_dev->mc_count, querier);
		}

		delta = im->timer.expires - jiffies;
		seq_printf(seq,
			   "\t\t\t\t%08X %5d %d:%08lX\t\t%d\n",
			   im->multiaddr, im->users,
			   im->tm_running,
			   im->tm_running ? jiffies_delta_to_clock_t(delta) : 0,
			   im->reporter);
	}
	return 0;
}