encode_apdu(struct sc_card *card, struct sc_apdu *plain, struct sc_apdu *sm,
		unsigned char *apdu_buf, size_t * apdu_buf_len)
{
	size_t block_size = 0;
	unsigned char dataTLV[4096] = { 0 };
	size_t data_tlv_len = 0;
	unsigned char le_tlv[256] = { 0 };
	size_t le_tlv_len = 0;
	size_t mac_tlv_len = 10;
	size_t tmp_lc = 0;
	size_t tmp_le = 0;
	unsigned char mac_tlv[256] = { 0 };
	epass2003_exdata *exdata = NULL;

	mac_tlv[0] = 0x8E;
	mac_tlv[1] = 8;
	/* size_t plain_le = 0; */
	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;
	exdata = (epass2003_exdata*)card->drv_data;
	block_size = (KEY_TYPE_DES == exdata->smtype ? 16 : 8);

	sm->cse = SC_APDU_CASE_4_SHORT;
	apdu_buf[0] = (unsigned char)plain->cla;
	apdu_buf[1] = (unsigned char)plain->ins;
	apdu_buf[2] = (unsigned char)plain->p1;
	apdu_buf[3] = (unsigned char)plain->p2;
	/* plain_le = plain->le; */
	/* padding */
	apdu_buf[4] = 0x80;
	memset(&apdu_buf[5], 0x00, block_size - 5);

	/* Data -> Data' */
	if (plain->lc != 0)
		if (0 != construct_data_tlv(card, plain, apdu_buf, dataTLV, &data_tlv_len, exdata->smtype))
			return -1;

	if (plain->le != 0 || (plain->le == 0 && plain->resplen != 0))
		if (0 != construct_le_tlv(plain, apdu_buf, data_tlv_len, le_tlv,
				     &le_tlv_len, exdata->smtype))
			return -1;

	if (0 != construct_mac_tlv(card, apdu_buf, data_tlv_len, le_tlv_len, mac_tlv, &mac_tlv_len, exdata->smtype))
		return -1;

	memset(apdu_buf + 4, 0, *apdu_buf_len - 4);
	sm->lc = sm->datalen = data_tlv_len + le_tlv_len + mac_tlv_len;
	if (sm->lc > 0xFF) {
		sm->cse = SC_APDU_CASE_4_EXT;
		apdu_buf[4] = (unsigned char)((sm->lc) / 0x10000);
		apdu_buf[5] = (unsigned char)(((sm->lc) / 0x100) % 0x100);
		apdu_buf[6] = (unsigned char)((sm->lc) % 0x100);
		tmp_lc = 3;
	}
	else {
		apdu_buf[4] = (unsigned char)sm->lc;
		tmp_lc = 1;
	}

	memcpy(apdu_buf + 4 + tmp_lc, dataTLV, data_tlv_len);
	memcpy(apdu_buf + 4 + tmp_lc + data_tlv_len, le_tlv, le_tlv_len);
	memcpy(apdu_buf + 4 + tmp_lc + data_tlv_len + le_tlv_len, mac_tlv, mac_tlv_len);
	memcpy((unsigned char *)sm->data, apdu_buf + 4 + tmp_lc, sm->datalen);
	*apdu_buf_len = 0;

	if (4 == le_tlv_len) {
		sm->cse = SC_APDU_CASE_4_EXT;
		*(apdu_buf + 4 + tmp_lc + sm->lc) = (unsigned char)(plain->le / 0x100);
		*(apdu_buf + 4 + tmp_lc + sm->lc + 1) = (unsigned char)(plain->le % 0x100);
		tmp_le = 2;
	}
	else if (3 == le_tlv_len) {
		*(apdu_buf + 4 + tmp_lc + sm->lc) = (unsigned char)plain->le;
		tmp_le = 1;
	}

	*apdu_buf_len += 4 + tmp_lc + data_tlv_len + le_tlv_len + mac_tlv_len + tmp_le;
	/* sm->le = calc_le(plain_le); */
	return 0;
}