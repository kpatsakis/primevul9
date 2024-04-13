static int packet_rcv_fanout(struct sk_buff *skb, struct net_device *dev,
			     struct packet_type *pt, struct net_device *orig_dev)
{
	struct packet_fanout *f = pt->af_packet_priv;
	unsigned int num = f->num_members;
	struct packet_sock *po;
	struct sock *sk;

	if (!net_eq(dev_net(dev), read_pnet(&f->net)) ||
	    !num) {
		kfree_skb(skb);
		return 0;
	}

	switch (f->type) {
	case PACKET_FANOUT_HASH:
	default:
		if (f->defrag) {
			skb = fanout_check_defrag(skb);
			if (!skb)
				return 0;
		}
		skb_get_rxhash(skb);
		sk = fanout_demux_hash(f, skb, num);
		break;
	case PACKET_FANOUT_LB:
		sk = fanout_demux_lb(f, skb, num);
		break;
	case PACKET_FANOUT_CPU:
		sk = fanout_demux_cpu(f, skb, num);
		break;
	}

	po = pkt_sk(sk);

	return po->prot_hook.func(skb, dev, &po->prot_hook, orig_dev);
}