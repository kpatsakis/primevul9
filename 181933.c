static u16 wcd9335_interp_get_primary_reg(u16 reg, u16 *ind)
{
	u16 prim_int_reg = WCD9335_CDC_RX0_RX_PATH_CTL;

	switch (reg) {
	case WCD9335_CDC_RX0_RX_PATH_CTL:
	case WCD9335_CDC_RX0_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX0_RX_PATH_CTL;
		*ind = 0;
		break;
	case WCD9335_CDC_RX1_RX_PATH_CTL:
	case WCD9335_CDC_RX1_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX1_RX_PATH_CTL;
		*ind = 1;
		break;
	case WCD9335_CDC_RX2_RX_PATH_CTL:
	case WCD9335_CDC_RX2_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX2_RX_PATH_CTL;
		*ind = 2;
		break;
	case WCD9335_CDC_RX3_RX_PATH_CTL:
	case WCD9335_CDC_RX3_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX3_RX_PATH_CTL;
		*ind = 3;
		break;
	case WCD9335_CDC_RX4_RX_PATH_CTL:
	case WCD9335_CDC_RX4_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX4_RX_PATH_CTL;
		*ind = 4;
		break;
	case WCD9335_CDC_RX5_RX_PATH_CTL:
	case WCD9335_CDC_RX5_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX5_RX_PATH_CTL;
		*ind = 5;
		break;
	case WCD9335_CDC_RX6_RX_PATH_CTL:
	case WCD9335_CDC_RX6_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX6_RX_PATH_CTL;
		*ind = 6;
		break;
	case WCD9335_CDC_RX7_RX_PATH_CTL:
	case WCD9335_CDC_RX7_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX7_RX_PATH_CTL;
		*ind = 7;
		break;
	case WCD9335_CDC_RX8_RX_PATH_CTL:
	case WCD9335_CDC_RX8_RX_PATH_MIX_CTL:
		prim_int_reg = WCD9335_CDC_RX8_RX_PATH_CTL;
		*ind = 8;
		break;
	};

	return prim_int_reg;
}