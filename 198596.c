epass2003_get_serialnr(struct sc_card *card, sc_serial_number_t * serial)
{
	u8 rbuf[8];
	size_t rbuf_len = sizeof(rbuf);

	LOG_FUNC_CALLED(card->ctx);

	if (SC_SUCCESS != get_data(card, 0x80, rbuf, rbuf_len))
		return SC_ERROR_CARD_CMD_FAILED;

	card->serialnr.len = serial->len = 8;
	memcpy(card->serialnr.value, rbuf, 8);
	memcpy(serial->value, rbuf, 8);

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}