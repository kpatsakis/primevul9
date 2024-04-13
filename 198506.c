epass2003_pin_cmd(struct sc_card *card, struct sc_pin_cmd_data *data, int *tries_left)
{
	int r;
	u8 kid;
	u8 retries = 0;
	u8 pin_low = 3;
	unsigned char maxtries = 0;

	LOG_FUNC_CALLED(card->ctx);

	internal_sanitize_pin_info(&data->pin1, 0);
	internal_sanitize_pin_info(&data->pin2, 1);
	data->flags |= SC_PIN_CMD_NEED_PADDING;
	kid = data->pin_reference;
	/* get pin retries */
	if (data->cmd == SC_PIN_CMD_GET_INFO) {

		r = get_external_key_retries(card, 0x80 | kid, &retries);
		if (r == SC_SUCCESS) {
			data->pin1.tries_left = retries;
			if (tries_left)
				*tries_left = retries;

			r = get_external_key_maxtries(card, &maxtries);
			LOG_TEST_RET(card->ctx, r, "get max counter failed");

			data->pin1.max_tries = maxtries;
		}
//remove below code, because the old implement only return PIN retries, now modify the code and return PIN status
//		return r;
	}
	else if (data->cmd == SC_PIN_CMD_UNBLOCK) { /* verify */
		r = external_key_auth(card, (kid + 1), (unsigned char *)data->pin1.data,
				data->pin1.len);
		LOG_TEST_RET(card->ctx, r, "verify pin failed");
	}
	else if (data->cmd == SC_PIN_CMD_CHANGE || data->cmd == SC_PIN_CMD_UNBLOCK) { /* change */
		r = update_secret_key(card, 0x04, kid, data->pin2.data,
				(unsigned long)data->pin2.len);
		LOG_TEST_RET(card->ctx, r, "verify pin failed");
	}
	else {
		r = external_key_auth(card, kid, (unsigned char *)data->pin1.data,
				data->pin1.len);
		get_external_key_retries(card, 0x80 | kid, &retries);
		if (retries < pin_low)
			sc_log(card->ctx, "Verification failed (remaining tries: %d)", retries);

	}
	LOG_TEST_RET(card->ctx, r, "verify pin failed");

	if (r == SC_SUCCESS)
	{
		data->pin1.logged_in = SC_PIN_STATE_LOGGED_IN;
	}

	return r;
}