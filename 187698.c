static unsigned char *add_mcs(unsigned char *bits, int bitrate,
			      unsigned int predef)
{
	const char *fw_name[2] = {FIRMWARE_9600, FIRMWARE_1200};
	const struct firmware *fw;
	struct platform_device *pdev;
	struct yam_mcs *p;
	int err;

	switch (predef) {
	case 0:
		fw = NULL;
		break;
	case YAM_1200:
	case YAM_9600:
		predef--;
		pdev = platform_device_register_simple("yam", 0, NULL, 0);
		if (IS_ERR(pdev)) {
			printk(KERN_ERR "yam: Failed to register firmware\n");
			return NULL;
		}
		err = request_firmware(&fw, fw_name[predef], &pdev->dev);
		platform_device_unregister(pdev);
		if (err) {
			printk(KERN_ERR "Failed to load firmware \"%s\"\n",
			       fw_name[predef]);
			return NULL;
		}
		if (fw->size != YAM_FPGA_SIZE) {
			printk(KERN_ERR "Bogus length %zu in firmware \"%s\"\n",
			       fw->size, fw_name[predef]);
			release_firmware(fw);
			return NULL;
		}
		bits = (unsigned char *)fw->data;
		break;
	default:
		printk(KERN_ERR "yam: Invalid predef number %u\n", predef);
		return NULL;
	}

	/* If it already exists, replace the bit data */
	p = yam_data;
	while (p) {
		if (p->bitrate == bitrate) {
			memcpy(p->bits, bits, YAM_FPGA_SIZE);
			goto out;
		}
		p = p->next;
	}

	/* Allocate a new mcs */
	if ((p = kmalloc(sizeof(struct yam_mcs), GFP_KERNEL)) == NULL) {
		release_firmware(fw);
		return NULL;
	}
	memcpy(p->bits, bits, YAM_FPGA_SIZE);
	p->bitrate = bitrate;
	p->next = yam_data;
	yam_data = p;
 out:
	release_firmware(fw);
	return p->bits;
}