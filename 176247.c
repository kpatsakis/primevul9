static int ax88179_check_eeprom(struct usbnet *dev)
{
	u8 i, buf, eeprom[20];
	u16 csum, delay = HZ / 10;
	unsigned long jtimeout;

	/* Read EEPROM content */
	for (i = 0; i < 6; i++) {
		buf = i;
		if (ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_SROM_ADDR,
				      1, 1, &buf) < 0)
			return -EINVAL;

		buf = EEP_RD;
		if (ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_SROM_CMD,
				      1, 1, &buf) < 0)
			return -EINVAL;

		jtimeout = jiffies + delay;
		do {
			ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_SROM_CMD,
					 1, 1, &buf);

			if (time_after(jiffies, jtimeout))
				return -EINVAL;

		} while (buf & EEP_BUSY);

		__ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_SROM_DATA_LOW,
				   2, 2, &eeprom[i * 2], 0);

		if ((i == 0) && (eeprom[0] == 0xFF))
			return -EINVAL;
	}

	csum = eeprom[6] + eeprom[7] + eeprom[8] + eeprom[9];
	csum = (csum >> 8) + (csum & 0xff);
	if ((csum + eeprom[10]) != 0xff)
		return -EINVAL;

	return 0;
}