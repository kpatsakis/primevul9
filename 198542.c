construct_data_tlv(struct sc_card *card, struct sc_apdu *apdu, unsigned char *apdu_buf,
		unsigned char *data_tlv, size_t * data_tlv_len, const unsigned char key_type)
{
	size_t block_size = (KEY_TYPE_AES == key_type ? 16 : 8);
	unsigned char pad[4096] = { 0 };
	size_t pad_len;
	size_t tlv_more;	/* increased tlv length */
	unsigned char iv[16] = { 0 };
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;

	exdata = (epass2003_exdata *)card->drv_data;

	/* padding */
	apdu_buf[block_size] = 0x87;
	memcpy(pad, apdu->data, apdu->lc);
	pad[apdu->lc] = 0x80;
	if ((apdu->lc + 1) % block_size)
		pad_len = ((apdu->lc + 1) / block_size + 1) * block_size;
	else
		pad_len = apdu->lc + 1;

	/* encode Lc' */
	if (pad_len > 0x7E) {
		/* Lc' > 0x7E, use extended APDU */
		apdu_buf[block_size + 1] = 0x82;
		apdu_buf[block_size + 2] = (unsigned char)((pad_len + 1) / 0x100);
		apdu_buf[block_size + 3] = (unsigned char)((pad_len + 1) % 0x100);
		apdu_buf[block_size + 4] = 0x01;
		tlv_more = 5;
	}
	else {
		apdu_buf[block_size + 1] = (unsigned char)pad_len + 1;
		apdu_buf[block_size + 2] = 0x01;
		tlv_more = 3;
	}
	memcpy(data_tlv, &apdu_buf[block_size], tlv_more);

	/* encrypt Data */
	if (KEY_TYPE_AES == key_type)
		aes128_encrypt_cbc(exdata->sk_enc, 16, iv, pad, pad_len, apdu_buf + block_size + tlv_more);
	else
		des3_encrypt_cbc(exdata->sk_enc, 16, iv, pad, pad_len, apdu_buf + block_size + tlv_more);

	memcpy(data_tlv + tlv_more, apdu_buf + block_size + tlv_more, pad_len);
	*data_tlv_len = tlv_more + pad_len;
	return 0;
}