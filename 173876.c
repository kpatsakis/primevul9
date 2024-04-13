static int hidpp10_battery_mileage_map_status(u8 param)
{
	int status;

	switch (param >> 6) {
	case 0x00:
		/* discharging (in use) */
		status = POWER_SUPPLY_STATUS_DISCHARGING;
		break;
	case 0x01: /* charging */
		status = POWER_SUPPLY_STATUS_CHARGING;
		break;
	case 0x02: /* charge complete */
		status = POWER_SUPPLY_STATUS_FULL;
		break;
	/*
	 * 0x03 = charging error
	 */
	default:
		status = POWER_SUPPLY_STATUS_NOT_CHARGING;
		break;
	}

	return status;
}