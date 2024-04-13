static int set_is_vf(int chip_id)
{
	switch (chip_id) {
	case BCM57712_VF:
	case BCM57800_VF:
	case BCM57810_VF:
	case BCM57840_VF:
	case BCM57811_VF:
		return true;
	default:
		return false;
	}
}