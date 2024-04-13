static int ttusb_dec_boot_dsp(struct ttusb_dec *dec)
{
	int i, j, actual_len, result, size, trans_count;
	u8 b0[] = { 0x00, 0x00, 0x00, 0x00,
		    0x00, 0x00, 0x00, 0x00,
		    0x61, 0x00 };
	u8 b1[] = { 0x61 };
	u8 *b;
	char idstring[21];
	const u8 *firmware = NULL;
	size_t firmware_size = 0;
	u16 firmware_csum = 0;
	__be16 firmware_csum_ns;
	__be32 firmware_size_nl;
	u32 crc32_csum, crc32_check;
	__be32 tmp;
	const struct firmware *fw_entry = NULL;

	dprintk("%s\n", __func__);

	result = request_firmware(&fw_entry, dec->firmware_name, &dec->udev->dev);
	if (result) {
		printk(KERN_ERR "%s: Firmware (%s) unavailable.\n",
		       __func__, dec->firmware_name);
		return result;
	}

	firmware = fw_entry->data;
	firmware_size = fw_entry->size;

	if (firmware_size < 60) {
		printk("%s: firmware size too small for DSP code (%zu < 60).\n",
			__func__, firmware_size);
		release_firmware(fw_entry);
		return -ENOENT;
	}

	/* a 32 bit checksum over the first 56 bytes of the DSP Code is stored
	   at offset 56 of file, so use it to check if the firmware file is
	   valid. */
	crc32_csum = crc32(~0L, firmware, 56) ^ ~0L;
	memcpy(&tmp, &firmware[56], 4);
	crc32_check = ntohl(tmp);
	if (crc32_csum != crc32_check) {
		printk("%s: crc32 check of DSP code failed (calculated 0x%08x != 0x%08x in file), file invalid.\n",
			__func__, crc32_csum, crc32_check);
		release_firmware(fw_entry);
		return -ENOENT;
	}
	memcpy(idstring, &firmware[36], 20);
	idstring[20] = '\0';
	printk(KERN_INFO "ttusb_dec: found DSP code \"%s\".\n", idstring);

	firmware_size_nl = htonl(firmware_size);
	memcpy(b0, &firmware_size_nl, 4);
	firmware_csum = crc16(~0, firmware, firmware_size) ^ ~0;
	firmware_csum_ns = htons(firmware_csum);
	memcpy(&b0[6], &firmware_csum_ns, 2);

	result = ttusb_dec_send_command(dec, 0x41, sizeof(b0), b0, NULL, NULL);

	if (result) {
		release_firmware(fw_entry);
		return result;
	}

	trans_count = 0;
	j = 0;

	b = kmalloc(ARM_PACKET_SIZE, GFP_KERNEL);
	if (b == NULL) {
		release_firmware(fw_entry);
		return -ENOMEM;
	}

	for (i = 0; i < firmware_size; i += COMMAND_PACKET_SIZE) {
		size = firmware_size - i;
		if (size > COMMAND_PACKET_SIZE)
			size = COMMAND_PACKET_SIZE;

		b[j + 0] = 0xaa;
		b[j + 1] = trans_count++;
		b[j + 2] = 0xf0;
		b[j + 3] = size;
		memcpy(&b[j + 4], &firmware[i], size);

		j += COMMAND_PACKET_SIZE + 4;

		if (j >= ARM_PACKET_SIZE) {
			result = usb_bulk_msg(dec->udev, dec->command_pipe, b,
					      ARM_PACKET_SIZE, &actual_len,
					      100);
			j = 0;
		} else if (size < COMMAND_PACKET_SIZE) {
			result = usb_bulk_msg(dec->udev, dec->command_pipe, b,
					      j - COMMAND_PACKET_SIZE + size,
					      &actual_len, 100);
		}
	}

	result = ttusb_dec_send_command(dec, 0x43, sizeof(b1), b1, NULL, NULL);

	release_firmware(fw_entry);
	kfree(b);

	return result;
}