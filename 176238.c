ax88179_set_eeprom(struct net_device *net, struct ethtool_eeprom *eeprom,
		   u8 *data)
{
	struct usbnet *dev = netdev_priv(net);
	u16 *eeprom_buff;
	int first_word;
	int last_word;
	int ret;
	int i;

	netdev_dbg(net, "write EEPROM len %d, offset %d, magic 0x%x\n",
		   eeprom->len, eeprom->offset, eeprom->magic);

	if (eeprom->len == 0)
		return -EINVAL;

	if (eeprom->magic != AX88179_EEPROM_MAGIC)
		return -EINVAL;

	first_word = eeprom->offset >> 1;
	last_word = (eeprom->offset + eeprom->len - 1) >> 1;

	eeprom_buff = kmalloc_array(last_word - first_word + 1, sizeof(u16),
				    GFP_KERNEL);
	if (!eeprom_buff)
		return -ENOMEM;

	/* align data to 16 bit boundaries, read the missing data from
	   the EEPROM */
	if (eeprom->offset & 1) {
		ret = ax88179_read_cmd(dev, AX_ACCESS_EEPROM, first_word, 1, 2,
				       &eeprom_buff[0]);
		if (ret < 0) {
			netdev_err(net, "Failed to read EEPROM at offset 0x%02x.\n", first_word);
			goto free;
		}
	}

	if ((eeprom->offset + eeprom->len) & 1) {
		ret = ax88179_read_cmd(dev, AX_ACCESS_EEPROM, last_word, 1, 2,
				       &eeprom_buff[last_word - first_word]);
		if (ret < 0) {
			netdev_err(net, "Failed to read EEPROM at offset 0x%02x.\n", last_word);
			goto free;
		}
	}

	memcpy((u8 *)eeprom_buff + (eeprom->offset & 1), data, eeprom->len);

	for (i = first_word; i <= last_word; i++) {
		netdev_dbg(net, "write to EEPROM at offset 0x%02x, data 0x%04x\n",
			   i, eeprom_buff[i - first_word]);
		ret = ax88179_write_cmd(dev, AX_ACCESS_EEPROM, i, 1, 2,
					&eeprom_buff[i - first_word]);
		if (ret < 0) {
			netdev_err(net, "Failed to write EEPROM at offset 0x%02x.\n", i);
			goto free;
		}
		msleep(20);
	}

	/* reload EEPROM data */
	ret = ax88179_write_cmd(dev, AX_RELOAD_EEPROM_EFUSE, 0x0000, 0, 0, NULL);
	if (ret < 0) {
		netdev_err(net, "Failed to reload EEPROM data\n");
		goto free;
	}

	ret = 0;
free:
	kfree(eeprom_buff);
	return ret;
}