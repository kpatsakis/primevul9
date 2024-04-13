static void __packet_set_status(struct packet_sock *po, void *frame, int status)
{
	union {
		struct tpacket_hdr *h1;
		struct tpacket2_hdr *h2;
		void *raw;
	} h;

	h.raw = frame;
	switch (po->tp_version) {
	case TPACKET_V1:
		h.h1->tp_status = status;
		flush_dcache_page(pgv_to_page(&h.h1->tp_status));
		break;
	case TPACKET_V2:
		h.h2->tp_status = status;
		flush_dcache_page(pgv_to_page(&h.h2->tp_status));
		break;
	case TPACKET_V3:
	default:
		WARN(1, "TPACKET version not supported.\n");
		BUG();
	}

	smp_wmb();
}