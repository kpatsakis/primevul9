static struct sock *fanout_demux_hash(struct packet_fanout *f, struct sk_buff *skb, unsigned int num)
{
	u32 idx, hash = skb->rxhash;

	idx = ((u64)hash * num) >> 32;

	return f->arr[idx];
}