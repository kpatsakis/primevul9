static bool ad5755_is_valid_mode(struct ad5755_state *st, enum ad5755_mode mode)
{
	switch (mode) {
	case AD5755_MODE_VOLTAGE_0V_5V:
	case AD5755_MODE_VOLTAGE_0V_10V:
	case AD5755_MODE_VOLTAGE_PLUSMINUS_5V:
	case AD5755_MODE_VOLTAGE_PLUSMINUS_10V:
		return st->chip_info->has_voltage_out;
	case AD5755_MODE_CURRENT_4mA_20mA:
	case AD5755_MODE_CURRENT_0mA_20mA:
	case AD5755_MODE_CURRENT_0mA_24mA:
		return true;
	default:
		return false;
	}
}