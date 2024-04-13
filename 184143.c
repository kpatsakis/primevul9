static __u32 __packet_set_timestamp(struct packet_sock *po, void *frame,
				    struct sk_buff *skb)
{
	union tpacket_uhdr h;
	struct timespec ts;
	__u32 ts_status;

	if (!(ts_status = tpacket_get_timestamp(skb, &ts, po->tp_tstamp)))
		return 0;

	h.raw = frame;
	switch (po->tp_version) {
	case TPACKET_V1:
		h.h1->tp_sec = ts.tv_sec;
		h.h1->tp_usec = ts.tv_nsec / NSEC_PER_USEC;
		break;
	case TPACKET_V2:
		h.h2->tp_sec = ts.tv_sec;
		h.h2->tp_nsec = ts.tv_nsec;
		break;
	case TPACKET_V3:
	default:
		WARN(1, "TPACKET version not supported.\n");
		BUG();
	}

	/* one flush is safe, as both fields always lie on the same cacheline */
	flush_dcache_page(pgv_to_page(&h.h1->tp_sec));
	smp_wmb();

	return ts_status;
}