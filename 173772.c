static u16 lg4ff_identify_multimode_wheel(struct hid_device *hid, const u16 reported_product_id, const u16 bcdDevice)
{
	u32 current_mode;
	int i;

	/* identify current mode from USB PID */
	for (i = 1; i < ARRAY_SIZE(lg4ff_alternate_modes); i++) {
		dbg_hid("Testing whether PID is %X\n", lg4ff_alternate_modes[i].product_id);
		if (reported_product_id == lg4ff_alternate_modes[i].product_id)
			break;
	}

	if (i == ARRAY_SIZE(lg4ff_alternate_modes))
		return 0;

	current_mode = BIT(i);

	for (i = 0; i < ARRAY_SIZE(lg4ff_main_checklist); i++) {
		const u16 mask = lg4ff_main_checklist[i]->mask;
		const u16 result = lg4ff_main_checklist[i]->result;
		const u16 real_product_id = lg4ff_main_checklist[i]->real_product_id;

		if ((current_mode & lg4ff_main_checklist[i]->modes) && \
				(bcdDevice & mask) == result) {
			dbg_hid("Found wheel with real PID %X whose reported PID is %X\n", real_product_id, reported_product_id);
			return real_product_id;
		}
	}

	/* No match found. This is either Driving Force or an unknown
	 * wheel model, do not touch it */
	dbg_hid("Wheel with bcdDevice %X was not recognized as multimode wheel, leaving in its current mode\n", bcdDevice);
	return 0;
}