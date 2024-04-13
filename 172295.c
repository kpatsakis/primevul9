static int my_pin_cmd(sc_card_t * card, struct sc_pin_cmd_data * data,
			int *tries_left) 
{
	/* GemSAFE pin uses a null terminated string with 0xFF */
	/* so we need to add the 0x00 to the pin  then pad with 0xFF */
	
	int r;
	const u8 *saved_data = NULL;
	int saved_len = 0;
	u8  newpin[8];
	
	LOG_FUNC_CALLED(card->ctx);

	memset(newpin, 0xff, sizeof(newpin));

	if (data->pin1.data && data->pin1.len < 8 && data->pin1.len > 0) {
		memcpy(newpin,data->pin1.data, (size_t)data->pin1.len);
		newpin[data->pin1.len] = 0x00;
		
		sc_log(card->ctx,  "pin len=%d", data->pin1.len);

		saved_data = data->pin1.data;
		saved_len = data->pin1.len;
		data->pin1.data = newpin;
		data->pin1.len = sizeof(newpin);
	}

	r = pin_cmd_save(card, data, tries_left);

	if (saved_data) {
		data->pin1.data = saved_data;
		data->pin1.len = saved_len;
	}

	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, r);
}