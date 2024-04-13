static int seq_show(struct seq_file *s, void *v)
{
	const struct nfqnl_instance *inst = v;

	return seq_printf(s, "%5d %6d %5d %1d %5d %5d %5d %8d %2d\n",
			  inst->queue_num,
			  inst->peer_portid, inst->queue_total,
			  inst->copy_mode, inst->copy_range,
			  inst->queue_dropped, inst->queue_user_dropped,
			  inst->id_sequence, 1);
}