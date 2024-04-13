llsec_update_devkey_info(struct mac802154_llsec_device *dev,
			 const struct ieee802154_llsec_key_id *in_key,
			 u32 frame_counter)
{
	struct mac802154_llsec_device_key *devkey = NULL;

	if (dev->dev.key_mode == IEEE802154_LLSEC_DEVKEY_RESTRICT) {
		devkey = llsec_devkey_find(dev, in_key);
		if (!devkey)
			return -ENOENT;
	}

	if (dev->dev.key_mode == IEEE802154_LLSEC_DEVKEY_RECORD) {
		int rc = llsec_update_devkey_record(dev, in_key);

		if (rc < 0)
			return rc;
	}

	spin_lock_bh(&dev->lock);

	if ((!devkey && frame_counter < dev->dev.frame_counter) ||
	    (devkey && frame_counter < devkey->devkey.frame_counter)) {
		spin_unlock_bh(&dev->lock);
		return -EINVAL;
	}

	if (devkey)
		devkey->devkey.frame_counter = frame_counter + 1;
	else
		dev->dev.frame_counter = frame_counter + 1;

	spin_unlock_bh(&dev->lock);

	return 0;
}