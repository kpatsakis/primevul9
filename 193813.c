static int setcos_putdata(struct sc_card *card, struct sc_cardctl_setcos_data_obj* data_obj)
{
	int				r;
	struct sc_apdu			apdu;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	memset(&apdu, 0, sizeof(apdu));
	apdu.cse     = SC_APDU_CASE_3_SHORT;
	apdu.cla     = 0x00;
	apdu.ins     = 0xDA;
	apdu.p1      = data_obj->P1;
	apdu.p2      = data_obj->P2;
	apdu.lc      = data_obj->DataLen;
	apdu.datalen = data_obj->DataLen;
	apdu.data    = data_obj->Data;

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "PUT_DATA returned error");

	LOG_FUNC_RETURN(card->ctx, r);
}