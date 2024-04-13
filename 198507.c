epass2003_sm_wrap_apdu(struct sc_card *card, struct sc_apdu *plain, struct sc_apdu *sm)
{
	unsigned char buf[4096] = { 0 };	/* APDU buffer */
	size_t buf_len = sizeof(buf);
	epass2003_exdata *exdata = NULL;
	
	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;
	
	exdata = (epass2003_exdata *)card->drv_data;

	LOG_FUNC_CALLED(card->ctx);

	if (exdata->sm)
		plain->cla |= 0x0C;

	sm->cse = plain->cse;
	sm->cla = plain->cla;
	sm->ins = plain->ins;
	sm->p1 = plain->p1;
	sm->p2 = plain->p2;
	sm->lc = plain->lc;
	sm->le = plain->le;
	sm->control = plain->control;
	sm->flags = plain->flags;

	switch (sm->cla & 0x0C) {
	case 0x00:
	case 0x04:
		sm->datalen = plain->datalen;
		memcpy((void *)sm->data, plain->data, plain->datalen);
		sm->resplen = plain->resplen;
		memcpy(sm->resp, plain->resp, plain->resplen);
		break;
	case 0x0C:
		memset(buf, 0, sizeof(buf));
		if (0 != encode_apdu(card, plain, sm, buf, &buf_len))
			return SC_ERROR_CARD_CMD_FAILED;
		break;
	default:
		return SC_ERROR_INCORRECT_PARAMETERS;
	}

	return SC_SUCCESS;
}