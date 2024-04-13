static int igmp_mcf_seq_show(struct seq_file *seq, void *v)
{
	struct ip_sf_list *psf = (struct ip_sf_list *)v;
	struct igmp_mcf_iter_state *state = igmp_mcf_seq_private(seq);

	if (v == SEQ_START_TOKEN) {
		seq_puts(seq, "Idx Device        MCA        SRC    INC    EXC\n");
	} else {
		seq_printf(seq,
			   "%3d %6.6s 0x%08x "
			   "0x%08x %6lu %6lu\n",
			   state->dev->ifindex, state->dev->name,
			   ntohl(state->im->multiaddr),
			   ntohl(psf->sf_inaddr),
			   psf->sf_count[MCAST_INCLUDE],
			   psf->sf_count[MCAST_EXCLUDE]);
	}
	return 0;
}