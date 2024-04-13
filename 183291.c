int csr_write_varid_valueless(int dd, uint16_t seqnum, uint16_t varid)
{
	unsigned char cmd[] = { 0x02, 0x00, 0x09, 0x00,
				seqnum & 0xff, seqnum >> 8, varid & 0xff, varid >> 8, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	unsigned char cp[254], rp[254];
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp[0] = 0xc2;
	memcpy(cp + 1, cmd, sizeof(cmd));

	switch (varid) {
	case CSR_VARID_COLD_RESET:
	case CSR_VARID_WARM_RESET:
	case CSR_VARID_COLD_HALT:
	case CSR_VARID_WARM_HALT:
		return hci_send_cmd(dd, OGF_VENDOR_CMD, 0x00, sizeof(cmd) + 1, cp);
	}

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_VENDOR_CMD;
	rq.ocf    = 0x00;
	rq.event  = EVT_VENDOR;
	rq.cparam = cp;
	rq.clen   = sizeof(cmd) + 1;
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