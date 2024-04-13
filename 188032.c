static int packet_getsockopt(struct socket *sock, int level, int optname,
			     char __user *optval, int __user *optlen)
{
	int len;
	int val;
	struct sock *sk = sock->sk;
	struct packet_sock *po = pkt_sk(sk);
	void *data;
	struct tpacket_stats st;
	union tpacket_stats_u st_u;

	if (level != SOL_PACKET)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	if (len < 0)
		return -EINVAL;

	switch (optname) {
	case PACKET_STATISTICS:
		if (po->tp_version == TPACKET_V3) {
			len = sizeof(struct tpacket_stats_v3);
		} else {
			if (len > sizeof(struct tpacket_stats))
				len = sizeof(struct tpacket_stats);
		}
		spin_lock_bh(&sk->sk_receive_queue.lock);
		if (po->tp_version == TPACKET_V3) {
			memcpy(&st_u.stats3, &po->stats,
			sizeof(struct tpacket_stats));
			st_u.stats3.tp_freeze_q_cnt =
			po->stats_u.stats3.tp_freeze_q_cnt;
			st_u.stats3.tp_packets += po->stats.tp_drops;
			data = &st_u.stats3;
		} else {
			st = po->stats;
			st.tp_packets += st.tp_drops;
			data = &st;
		}
		memset(&po->stats, 0, sizeof(st));
		spin_unlock_bh(&sk->sk_receive_queue.lock);
		break;
	case PACKET_AUXDATA:
		if (len > sizeof(int))
			len = sizeof(int);
		val = po->auxdata;

		data = &val;
		break;
	case PACKET_ORIGDEV:
		if (len > sizeof(int))
			len = sizeof(int);
		val = po->origdev;

		data = &val;
		break;
	case PACKET_VNET_HDR:
		if (len > sizeof(int))
			len = sizeof(int);
		val = po->has_vnet_hdr;

		data = &val;
		break;
	case PACKET_VERSION:
		if (len > sizeof(int))
			len = sizeof(int);
		val = po->tp_version;
		data = &val;
		break;
	case PACKET_HDRLEN:
		if (len > sizeof(int))
			len = sizeof(int);
		if (copy_from_user(&val, optval, len))
			return -EFAULT;
		switch (val) {
		case TPACKET_V1:
			val = sizeof(struct tpacket_hdr);
			break;
		case TPACKET_V2:
			val = sizeof(struct tpacket2_hdr);
			break;
		case TPACKET_V3:
			val = sizeof(struct tpacket3_hdr);
			break;
		default:
			return -EINVAL;
		}
		data = &val;
		break;
	case PACKET_RESERVE:
		if (len > sizeof(unsigned int))
			len = sizeof(unsigned int);
		val = po->tp_reserve;
		data = &val;
		break;
	case PACKET_LOSS:
		if (len > sizeof(unsigned int))
			len = sizeof(unsigned int);
		val = po->tp_loss;
		data = &val;
		break;
	case PACKET_TIMESTAMP:
		if (len > sizeof(int))
			len = sizeof(int);
		val = po->tp_tstamp;
		data = &val;
		break;
	case PACKET_FANOUT:
		if (len > sizeof(int))
			len = sizeof(int);
		val = (po->fanout ?
		       ((u32)po->fanout->id |
			((u32)po->fanout->type << 16)) :
		       0);
		data = &val;
		break;
	default:
		return -ENOPROTOOPT;
	}

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, data, len))
		return -EFAULT;
	return 0;
}