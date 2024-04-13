static ssize_t ds4_store_poll_interval(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *sc = hid_get_drvdata(hdev);
	unsigned long flags;
	u8 interval;

	if (kstrtou8(buf, 0, &interval))
		return -EINVAL;

	if (interval > DS4_BT_MAX_POLL_INTERVAL_MS)
		return -EINVAL;

	spin_lock_irqsave(&sc->lock, flags);
	sc->ds4_bt_poll_interval = interval;
	spin_unlock_irqrestore(&sc->lock, flags);

	sony_schedule_work(sc, SONY_WORKER_STATE);

	return count;
}