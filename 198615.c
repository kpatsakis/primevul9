internal_sanitize_pin_info(struct sc_pin_cmd_pin *pin, unsigned int num)
{
	pin->encoding = SC_PIN_ENCODING_ASCII;
	pin->min_length = 4;
	pin->max_length = 16;
	pin->pad_length = 16;
	pin->offset = 5 + num * 16;
	pin->pad_char = 0x00;
}