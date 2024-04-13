static ssize_t is_valid_bcd(u16 bcd_val)
{
	if ((bcd_val & 0xf) > 9)
		return -EINVAL;
	if (((bcd_val >> 4) & 0xf) > 9)
		return -EINVAL;
	if (((bcd_val >> 8) & 0xf) > 9)
		return -EINVAL;
	if (((bcd_val >> 12) & 0xf) > 9)
		return -EINVAL;
	return 0;
}