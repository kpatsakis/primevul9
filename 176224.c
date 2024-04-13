ax88179_get_eeprom(struct net_device *net, struct ethtool_eeprom *eeprom,
		   u8 *data)
{
	struct usbnet *dev = netdev_priv(net);
	u16 *eeprom_buff;
	int first_word, last_word;
	int i, ret;

	if (eeprom->len == 0)
		return -EINVAL;

	eeprom->magic = AX88179_EEPROM_MAGIC;

	first_word = eeprom->offset >> 1;
	last_word = (eeprom->offset + eeprom->len - 1) >> 1;
	eeprom_buff = kmalloc_array(last_word - first_word + 1, sizeof(u16),
				    GFP_KERNEL);
	if (!eeprom_buff)
		return -ENOMEM;

	/* ax88179/178A returns 2 bytes from eeprom on read */
	for (i = first_word; i <= last_word; i++) {
		ret = __ax88179_read_cmd(dev, AX_ACCESS_EEPROM, i, 1, 2,
					 &eeprom_buff[i - first_word],
					 0);
		if (ret < 0) {
			kfree(eeprom_buff);
			return -EIO;
		}
	}

	memcpy(data, (u8 *)eeprom_buff + (eeprom->offset & 1), eeprom->len);
	kfree(eeprom_buff);
	return 0;
}