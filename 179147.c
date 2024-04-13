static int nfc_se_io(struct nfc_dev *dev, u32 se_idx,
		     u8 *apdu, size_t apdu_length,
		     se_io_cb_t cb, void *cb_context)
{
	struct nfc_se *se;
	int rc;

	pr_debug("%s se index %d\n", dev_name(&dev->dev), se_idx);

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (!dev->dev_up) {
		rc = -ENODEV;
		goto error;
	}

	if (!dev->ops->se_io) {
		rc = -EOPNOTSUPP;
		goto error;
	}

	se = nfc_find_se(dev, se_idx);
	if (!se) {
		rc = -EINVAL;
		goto error;
	}

	if (se->state != NFC_SE_ENABLED) {
		rc = -ENODEV;
		goto error;
	}

	rc = dev->ops->se_io(dev, se_idx, apdu,
			apdu_length, cb, cb_context);

error:
	device_unlock(&dev->dev);
	return rc;
}