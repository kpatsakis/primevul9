static int ttusb_dec_init_stb(struct ttusb_dec *dec)
{
	int result;
	unsigned int mode = 0, model = 0, version = 0;

	dprintk("%s\n", __func__);

	result = ttusb_dec_get_stb_state(dec, &mode, &model, &version);
	if (result)
		return result;

	if (!mode) {
		if (version == 0xABCDEFAB)
			printk(KERN_INFO "ttusb_dec: no version info in Firmware\n");
		else
			printk(KERN_INFO "ttusb_dec: Firmware %x.%02x%c%c\n",
			       version >> 24, (version >> 16) & 0xff,
			       (version >> 8) & 0xff, version & 0xff);

		result = ttusb_dec_boot_dsp(dec);
		if (result)
			return result;
	} else {
		/* We can't trust the USB IDs that some firmwares
		   give the box */
		switch (model) {
		case 0x00070001:
		case 0x00070008:
		case 0x0007000c:
			ttusb_dec_set_model(dec, TTUSB_DEC3000S);
			break;
		case 0x00070009:
		case 0x00070013:
			ttusb_dec_set_model(dec, TTUSB_DEC2000T);
			break;
		case 0x00070011:
			ttusb_dec_set_model(dec, TTUSB_DEC2540T);
			break;
		default:
			printk(KERN_ERR "%s: unknown model returned by firmware (%08x) - please report\n",
			       __func__, model);
			return -ENOENT;
		}
		if (version >= 0x01770000)
			dec->can_playback = 1;
	}
	return 0;
}