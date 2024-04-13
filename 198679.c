construct_le_tlv(struct sc_apdu *apdu, unsigned char *apdu_buf, size_t data_tlv_len,
		unsigned char *le_tlv, size_t * le_tlv_len, const unsigned char key_type)
{
	size_t block_size = (KEY_TYPE_AES == key_type ? 16 : 8);

	*(apdu_buf + block_size + data_tlv_len) = 0x97;
	if (apdu->le > 0x7F) {
		/* Le' > 0x7E, use extended APDU */
		*(apdu_buf + block_size + data_tlv_len + 1) = 2;
		*(apdu_buf + block_size + data_tlv_len + 2) = (unsigned char)(apdu->le / 0x100);
		*(apdu_buf + block_size + data_tlv_len + 3) = (unsigned char)(apdu->le % 0x100);
		memcpy(le_tlv, apdu_buf + block_size + data_tlv_len, 4);
		*le_tlv_len = 4;
	}
	else {
		*(apdu_buf + block_size + data_tlv_len + 1) = 1;
		*(apdu_buf + block_size + data_tlv_len + 2) = (unsigned char)apdu->le;
		memcpy(le_tlv, apdu_buf + block_size + data_tlv_len, 3);
		*le_tlv_len = 3;
	}
	return 0;
}