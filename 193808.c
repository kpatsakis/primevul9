static int setcos_getdata(struct sc_card *card, struct sc_cardctl_setcos_data_obj* data_obj)
{
	int				r;
	struct sc_apdu			apdu;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	memset(&apdu, 0, sizeof(apdu));
	apdu.cse     = SC_APDU_CASE_2_SHORT;
	apdu.cla     = 0x00;
	apdu.ins     = 0xCA;			/* GET DATA */
	apdu.p1      = data_obj->P1;
	apdu.p2      = data_obj->P2;
	apdu.lc      = 0;
	apdu.datalen = 0;
	apdu.data    = data_obj->Data;

	apdu.le      = 256;
	apdu.resp    = data_obj->Data;
	apdu.resplen = data_obj->DataLen;

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "GET_DATA returned error");

	if (apdu.resplen > data_obj->DataLen)
		r = SC_ERROR_WRONG_LENGTH;
	else
		data_obj->DataLen = apdu.resplen;

	LOG_FUNC_RETURN(card->ctx, r);
}