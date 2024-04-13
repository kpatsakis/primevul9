static int generate_key(void)
{
	sc_apdu_t apdu;
	u8 sbuf[4];
	u8 p2;
	int r;

	switch (opt_mod_length) {
	case 512:
		p2 = 0x40;
		break;
	case 768:
		p2 = 0x60;
		break;
	case 1024:
		p2 = 0x80;
		break;
	case 2048:
		p2 = 0x00;
		break;
	default:
		fprintf(stderr, "Invalid modulus length.\n");
		return 2;
	}
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0x46, (u8) opt_key_num-1, p2);
	apdu.cla = 0xF0;
	apdu.lc = 4;
	apdu.datalen = 4;
	apdu.data = sbuf;
	sbuf[0] = opt_exponent & 0xFF;
	sbuf[1] = (opt_exponent >> 8) & 0xFF;
	sbuf[2] = (opt_exponent >> 16) & 0xFF;
	sbuf[3] = (opt_exponent >> 24) & 0xFF;
	r = select_app_df();
	if (r)
		return 1;
	if (verbose)
		printf("Generating key...\n");
	r = sc_transmit_apdu(card, &apdu);
	if (r) {
		fprintf(stderr, "APDU transmit failed: %s\n", sc_strerror(r));
		if (r == SC_ERROR_TRANSMIT_FAILED)
			fprintf(stderr, "Reader has timed out. It is still possible that the key generation has\n"
					"succeeded.\n");
		return 1;
	}
	if (apdu.sw1 == 0x90 && apdu.sw2 == 0x00) {
		printf("Key generation successful.\n");
		return 0;
	}
	if (apdu.sw1 == 0x69 && apdu.sw2 == 0x82)
		fprintf(stderr, "CHV1 not verified or invalid exponent value.\n");
	else
		fprintf(stderr, "Card returned SW1=%02X, SW2=%02X.\n", apdu.sw1, apdu.sw2);
	return 1;
}