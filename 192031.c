int rtl8xxxu_load_firmware(struct rtl8xxxu_priv *priv, char *fw_name)
{
	struct device *dev = &priv->udev->dev;
	const struct firmware *fw;
	int ret = 0;
	u16 signature;

	dev_info(dev, "%s: Loading firmware %s\n", DRIVER_NAME, fw_name);
	if (request_firmware(&fw, fw_name, &priv->udev->dev)) {
		dev_warn(dev, "request_firmware(%s) failed\n", fw_name);
		ret = -EAGAIN;
		goto exit;
	}
	if (!fw) {
		dev_warn(dev, "Firmware data not available\n");
		ret = -EINVAL;
		goto exit;
	}

	priv->fw_data = kmemdup(fw->data, fw->size, GFP_KERNEL);
	if (!priv->fw_data) {
		ret = -ENOMEM;
		goto exit;
	}
	priv->fw_size = fw->size - sizeof(struct rtl8xxxu_firmware_header);

	signature = le16_to_cpu(priv->fw_data->signature);
	switch (signature & 0xfff0) {
	case 0x92e0:
	case 0x92c0:
	case 0x88c0:
	case 0x5300:
	case 0x2300:
		break;
	default:
		ret = -EINVAL;
		dev_warn(dev, "%s: Invalid firmware signature: 0x%04x\n",
			 __func__, signature);
	}

	dev_info(dev, "Firmware revision %i.%i (signature 0x%04x)\n",
		 le16_to_cpu(priv->fw_data->major_version),
		 priv->fw_data->minor_version, signature);

exit:
	release_firmware(fw);
	return ret;
}