static int atalk_seq_interface_show(struct seq_file *seq, void *v)
{
	struct atalk_iface *iface;

	if (v == SEQ_START_TOKEN) {
		seq_puts(seq, "Interface        Address   Networks  "
			      "Status\n");
		goto out;
	}

	iface = v;
	seq_printf(seq, "%-16s %04X:%02X  %04X-%04X  %d\n",
		   iface->dev->name, ntohs(iface->address.s_net),
		   iface->address.s_node, ntohs(iface->nets.nr_firstnet),
		   ntohs(iface->nets.nr_lastnet), iface->status);
out:
	return 0;
}