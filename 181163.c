static bool ad5755_is_voltage_mode(enum ad5755_mode mode)
{
	switch (mode) {
	case AD5755_MODE_VOLTAGE_0V_5V:
	case AD5755_MODE_VOLTAGE_0V_10V:
	case AD5755_MODE_VOLTAGE_PLUSMINUS_5V:
	case AD5755_MODE_VOLTAGE_PLUSMINUS_10V:
		return true;
	default:
		return false;
	}
}