static int ax88179_check_efuse(struct usbnet *dev, u16 *ledmode)
{
	u8	i;
	u8	efuse[64];
	u16	csum = 0;

	if (ax88179_read_cmd(dev, AX_ACCESS_EFUS, 0, 64, 64, efuse) < 0)
		return -EINVAL;

	if (*efuse == 0xFF)
		return -EINVAL;

	for (i = 0; i < 64; i++)
		csum = csum + efuse[i];

	while (csum > 255)
		csum = (csum & 0x00FF) + ((csum >> 8) & 0x00FF);

	if (csum != 0xFF)
		return -EINVAL;

	*ledmode = (efuse[51] << 8) | efuse[52];

	return 0;
}