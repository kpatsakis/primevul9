static int __packet_get_status(struct packet_sock *po, void *frame)
{
	union tpacket_uhdr h;

	smp_rmb();

	h.raw = frame;
	switch (po->tp_version) {
	case TPACKET_V1:
		flush_dcache_page(pgv_to_page(&h.h1->tp_status));
		return h.h1->tp_status;
	case TPACKET_V2:
		flush_dcache_page(pgv_to_page(&h.h2->tp_status));
		return h.h2->tp_status;
	case TPACKET_V3:
	default:
		WARN(1, "TPACKET version not supported.\n");
		BUG();
		return 0;
	}
}