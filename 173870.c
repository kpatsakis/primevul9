static int sony_check_add_dev_list(struct sony_sc *sc)
{
	struct sony_sc *entry;
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sony_dev_list_lock, flags);

	list_for_each_entry(entry, &sony_device_list, list_node) {
		ret = memcmp(sc->mac_address, entry->mac_address,
				sizeof(sc->mac_address));
		if (!ret) {
			if (sony_compare_connection_type(sc, entry)) {
				ret = 1;
			} else {
				ret = -EEXIST;
				hid_info(sc->hdev,
				"controller with MAC address %pMR already connected\n",
				sc->mac_address);
			}
			goto unlock;
		}
	}

	ret = 0;
	list_add(&(sc->list_node), &sony_device_list);

unlock:
	spin_unlock_irqrestore(&sony_dev_list_lock, flags);
	return ret;
}