static int aarp_seq_show(struct seq_file *seq, void *v)
{
	struct aarp_iter_state *iter = seq->private;
	struct aarp_entry *entry = v;
	unsigned long now = jiffies;

	if (v == SEQ_START_TOKEN)
		seq_puts(seq,
			 "Address  Interface   Hardware Address"
			 "   Expires LastSend  Retry Status\n");
	else {
		seq_printf(seq, "%04X:%02X  %-12s",
			   ntohs(entry->target_addr.s_net),
			   (unsigned int) entry->target_addr.s_node,
			   entry->dev ? entry->dev->name : "????");
		seq_printf(seq, "%pM", entry->hwaddr);
		seq_printf(seq, " %8s",
			   dt2str((long)entry->expires_at - (long)now));
		if (iter->table == unresolved)
			seq_printf(seq, " %8s %6hu",
				   dt2str(now - entry->last_sent),
				   entry->xmit_count);
		else
			seq_puts(seq, "                ");
		seq_printf(seq, " %s\n",
			   (iter->table == resolved) ? "resolved"
			   : (iter->table == unresolved) ? "unresolved"
			   : (iter->table == proxies) ? "proxies"
			   : "unknown");
	}
	return 0;
}