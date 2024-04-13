install_secret_key(struct sc_card *card, unsigned char ktype, unsigned char kid,
		unsigned char useac, unsigned char modifyac, unsigned char EC,
		unsigned char *data, unsigned long dataLen)
{
	int r;
	struct sc_apdu apdu;
	unsigned char isapp = 0x00;	/* appendable */
	unsigned char tmp_data[256] = { 0 };

	tmp_data[0] = ktype;
	tmp_data[1] = kid;
	tmp_data[2] = useac;
	tmp_data[3] = modifyac;
	tmp_data[8] = 0xFF;

	if (0x04 == ktype || 0x06 == ktype) {
		tmp_data[4] = EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_SO;
		tmp_data[5] = EPASS2003_AC_MAC_NOLESS | EPASS2003_AC_SO;
		tmp_data[7] = (kid == PIN_ID[0] ? EPASS2003_AC_USER : EPASS2003_AC_SO);
		tmp_data[9] = (EC << 4) | EC;
	}

	memcpy(&tmp_data[10], data, dataLen);
	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xe3, isapp, 0x00);
	apdu.cla = 0x80;
	apdu.lc = apdu.datalen = 10 + dataLen;
	apdu.data = tmp_data;

	r = sc_transmit_apdu_t(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU install_secret_key failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "install_secret_key failed");

	return r;
}