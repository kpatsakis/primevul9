static int tpacket_parse_header(struct packet_sock *po, void *frame,
				int size_max, void **data)
{
	union tpacket_uhdr ph;
	int tp_len, off;

	ph.raw = frame;

	switch (po->tp_version) {
	case TPACKET_V2:
		tp_len = ph.h2->tp_len;
		break;
	default:
		tp_len = ph.h1->tp_len;
		break;
	}
	if (unlikely(tp_len > size_max)) {
		pr_err("packet size is too long (%d > %d)\n", tp_len, size_max);
		return -EMSGSIZE;
	}

	if (unlikely(po->tp_tx_has_off)) {
		int off_min, off_max;

		off_min = po->tp_hdrlen - sizeof(struct sockaddr_ll);
		off_max = po->tx_ring.frame_size - tp_len;
		if (po->sk.sk_type == SOCK_DGRAM) {
			switch (po->tp_version) {
			case TPACKET_V2:
				off = ph.h2->tp_net;
				break;
			default:
				off = ph.h1->tp_net;
				break;
			}
		} else {
			switch (po->tp_version) {
			case TPACKET_V2:
				off = ph.h2->tp_mac;
				break;
			default:
				off = ph.h1->tp_mac;
				break;
			}
		}
		if (unlikely((off < off_min) || (off_max < off)))
			return -EINVAL;
	} else {
		off = po->tp_hdrlen - sizeof(struct sockaddr_ll);
	}

	*data = frame + off;
	return tp_len;
}