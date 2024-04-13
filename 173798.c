static int hidpp10_battery_status_map_level(u8 param)
{
	int level;

	switch (param) {
	case 1 ... 2:
		level = POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL;
		break;
	case 3 ... 4:
		level = POWER_SUPPLY_CAPACITY_LEVEL_LOW;
		break;
	case 5 ... 6:
		level = POWER_SUPPLY_CAPACITY_LEVEL_NORMAL;
		break;
	case 7:
		level = POWER_SUPPLY_CAPACITY_LEVEL_HIGH;
		break;
	default:
		level = POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN;
	}

	return level;
}