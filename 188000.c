static struct sock *fanout_demux_cpu(struct packet_fanout *f, struct sk_buff *skb, unsigned int num)
{
	unsigned int cpu = smp_processor_id();

	return f->arr[cpu % num];
}