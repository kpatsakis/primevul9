int lg4ff_raw_event(struct hid_device *hdev, struct hid_report *report,
		u8 *rd, int size, struct lg_drv_data *drv_data)
{
	int offset;
	struct lg4ff_device_entry *entry = drv_data->device_props;

	if (!entry)
		return 0;

	/* adjust HID report present combined pedals data */
	if (entry->wdata.combine) {
		switch (entry->wdata.product_id) {
		case USB_DEVICE_ID_LOGITECH_WHEEL:
			rd[5] = rd[3];
			rd[6] = 0x7F;
			return 1;
		case USB_DEVICE_ID_LOGITECH_WINGMAN_FG:
		case USB_DEVICE_ID_LOGITECH_WINGMAN_FFG:
		case USB_DEVICE_ID_LOGITECH_MOMO_WHEEL:
		case USB_DEVICE_ID_LOGITECH_MOMO_WHEEL2:
			rd[4] = rd[3];
			rd[5] = 0x7F;
			return 1;
		case USB_DEVICE_ID_LOGITECH_DFP_WHEEL:
			rd[5] = rd[4];
			rd[6] = 0x7F;
			return 1;
		case USB_DEVICE_ID_LOGITECH_G25_WHEEL:
		case USB_DEVICE_ID_LOGITECH_G27_WHEEL:
			offset = 5;
			break;
		case USB_DEVICE_ID_LOGITECH_DFGT_WHEEL:
		case USB_DEVICE_ID_LOGITECH_G29_WHEEL:
			offset = 6;
			break;
		case USB_DEVICE_ID_LOGITECH_WII_WHEEL:
			offset = 3;
			break;
		default:
			return 0;
		}

		/* Compute a combined axis when wheel does not supply it */
		rd[offset] = (0xFF + rd[offset] - rd[offset+1]) >> 1;
		rd[offset+1] = 0x7F;
		return 1;
	}

	return 0;
}