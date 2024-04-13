static struct sock *fanout_demux_lb(struct packet_fanout *f, struct sk_buff *skb, unsigned int num)
{
	int cur, old;

	cur = atomic_read(&f->rr_cur);
	while ((old = atomic_cmpxchg(&f->rr_cur, cur,
				     fanout_rr_next(f, num))) != cur)
		cur = old;
	return f->arr[cur];
}