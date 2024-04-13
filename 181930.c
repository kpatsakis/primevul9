static bool wcd9335_is_volatile_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case WCD9335_INTR_PIN1_STATUS0...WCD9335_INTR_PIN2_CLEAR3:
	case WCD9335_ANA_MBHC_RESULT_3:
	case WCD9335_ANA_MBHC_RESULT_2:
	case WCD9335_ANA_MBHC_RESULT_1:
	case WCD9335_ANA_MBHC_MECH:
	case WCD9335_ANA_MBHC_ELECT:
	case WCD9335_ANA_MBHC_ZDET:
	case WCD9335_ANA_MICB2:
	case WCD9335_ANA_RCO:
	case WCD9335_ANA_BIAS:
		return true;
	default:
		return false;
	}
}