static void dualshock4_set_leds_from_id(struct sony_sc *sc)
{
	/* The first 4 color/index entries match what the PS4 assigns */
	static const u8 color_code[7][3] = {
			/* Blue   */	{ 0x00, 0x00, 0x40 },
			/* Red	  */	{ 0x40, 0x00, 0x00 },
			/* Green  */	{ 0x00, 0x40, 0x00 },
			/* Pink   */	{ 0x20, 0x00, 0x20 },
			/* Orange */	{ 0x02, 0x01, 0x00 },
			/* Teal   */	{ 0x00, 0x01, 0x01 },
			/* White  */	{ 0x01, 0x01, 0x01 }
	};

	int id = sc->device_id;

	BUILD_BUG_ON(MAX_LEDS < ARRAY_SIZE(color_code[0]));

	if (id < 0)
		return;

	id %= 7;
	memcpy(sc->led_state, color_code[id], sizeof(color_code[id]));
}