static void sixaxis_parse_report(struct sony_sc *sc, u8 *rd, int size)
{
	static const u8 sixaxis_battery_capacity[] = { 0, 1, 25, 50, 75, 100 };
	unsigned long flags;
	int offset;
	u8 cable_state, battery_capacity, battery_charging;

	/*
	 * The sixaxis is charging if the battery value is 0xee
	 * and it is fully charged if the value is 0xef.
	 * It does not report the actual level while charging so it
	 * is set to 100% while charging is in progress.
	 */
	offset = (sc->quirks & MOTION_CONTROLLER) ? 12 : 30;

	if (rd[offset] >= 0xee) {
		battery_capacity = 100;
		battery_charging = !(rd[offset] & 0x01);
		cable_state = 1;
	} else {
		u8 index = rd[offset] <= 5 ? rd[offset] : 5;
		battery_capacity = sixaxis_battery_capacity[index];
		battery_charging = 0;
		cable_state = 0;
	}

	spin_lock_irqsave(&sc->lock, flags);
	sc->cable_state = cable_state;
	sc->battery_capacity = battery_capacity;
	sc->battery_charging = battery_charging;
	spin_unlock_irqrestore(&sc->lock, flags);

	if (sc->quirks & SIXAXIS_CONTROLLER) {
		int val;

		offset = SIXAXIS_INPUT_REPORT_ACC_X_OFFSET;
		val = ((rd[offset+1] << 8) | rd[offset]) - 511;
		input_report_abs(sc->sensor_dev, ABS_X, val);

		/* Y and Z are swapped and inversed */
		val = 511 - ((rd[offset+5] << 8) | rd[offset+4]);
		input_report_abs(sc->sensor_dev, ABS_Y, val);

		val = 511 - ((rd[offset+3] << 8) | rd[offset+2]);
		input_report_abs(sc->sensor_dev, ABS_Z, val);

		input_sync(sc->sensor_dev);
	}
}