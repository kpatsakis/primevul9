static int set_max_cos_est(int chip_id)
{
	switch (chip_id) {
	case BCM57710:
	case BCM57711:
	case BCM57711E:
		return BNX2X_MULTI_TX_COS_E1X;
	case BCM57712:
	case BCM57712_MF:
		return BNX2X_MULTI_TX_COS_E2_E3A0;
	case BCM57800:
	case BCM57800_MF:
	case BCM57810:
	case BCM57810_MF:
	case BCM57840_4_10:
	case BCM57840_2_20:
	case BCM57840_O:
	case BCM57840_MFO:
	case BCM57840_MF:
	case BCM57811:
	case BCM57811_MF:
		return BNX2X_MULTI_TX_COS_E3B0;
	case BCM57712_VF:
	case BCM57800_VF:
	case BCM57810_VF:
	case BCM57840_VF:
	case BCM57811_VF:
		return 1;
	default:
		pr_err("Unknown board_type (%d), aborting\n", chip_id);
		return -ENODEV;
	}
}