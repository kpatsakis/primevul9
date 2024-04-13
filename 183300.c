int csr_write_pskey_complex(int dd, uint16_t seqnum, uint16_t pskey, uint16_t stores, uint8_t *value, uint16_t length)
{
	unsigned char cmd[] = { 0x02, 0x00, ((length / 2) + 8) & 0xff, ((length / 2) + 8) >> 8,
				seqnum & 0xff, seqnum >> 8, 0x03, 0x70, 0x00, 0x00,
				pskey & 0xff, pskey >> 8,
				(length / 2) & 0xff, (length / 2) >> 8,
				stores & 0xff, stores >> 8, 0x00, 0x00 };

	unsigned char cp[254], rp[254];
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp[0] = 0xc2;
	memcpy(cp + 1, cmd, sizeof(cmd));

	memcpy(cp + 17, value, length);

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_VENDOR_CMD;
	rq.ocf    = 0x00;
	rq.event  = EVT_VENDOR;
	rq.cparam = cp;
	rq.clen   = sizeof(cmd) + length - 1;
	rq.rparam = rp;
	rq.rlen   = sizeof(rp);

	if (hci_send_req(dd, &rq, 2000) < 0)
		return -1;

	if (rp[0] != 0xc2) {
		errno = EIO;
		return -1;
	}

	if ((rp[9] + (rp[10] << 8)) != 0) {
		errno = ENXIO;
		return -1;
	}

	return 0;
}