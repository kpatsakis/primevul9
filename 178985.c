static void *atalk_seq_interface_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct atalk_iface *i;

	++*pos;
	if (v == SEQ_START_TOKEN) {
		i = NULL;
		if (atalk_interfaces)
			i = atalk_interfaces;
		goto out;
	}
	i = v;
	i = i->next;
out:
	return i;
}