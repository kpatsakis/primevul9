static int sony_raw_event(struct hid_device *hdev, struct hid_report *report,
		u8 *rd, int size)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	/*
	 * Sixaxis HID report has acclerometers/gyro with MSByte first, this
	 * has to be BYTE_SWAPPED before passing up to joystick interface
	 */
	if ((sc->quirks & SIXAXIS_CONTROLLER) && rd[0] == 0x01 && size == 49) {
		/*
		 * When connected via Bluetooth the Sixaxis occasionally sends
		 * a report with the second byte 0xff and the rest zeroed.
		 *
		 * This report does not reflect the actual state of the
		 * controller must be ignored to avoid generating false input
		 * events.
		 */
		if (rd[1] == 0xff)
			return -EINVAL;

		swap(rd[41], rd[42]);
		swap(rd[43], rd[44]);
		swap(rd[45], rd[46]);
		swap(rd[47], rd[48]);

		sixaxis_parse_report(sc, rd, size);
	} else if ((sc->quirks & MOTION_CONTROLLER_BT) && rd[0] == 0x01 && size == 49) {
		sixaxis_parse_report(sc, rd, size);
	} else if ((sc->quirks & NAVIGATION_CONTROLLER) && rd[0] == 0x01 &&
			size == 49) {
		sixaxis_parse_report(sc, rd, size);
	} else if ((sc->quirks & DUALSHOCK4_CONTROLLER_USB) && rd[0] == 0x01 &&
			size == 64) {
		dualshock4_parse_report(sc, rd, size);
	} else if (((sc->quirks & DUALSHOCK4_CONTROLLER_BT) && rd[0] == 0x11 &&
			size == 78)) {
		/* CRC check */
		u8 bthdr = 0xA1;
		u32 crc;
		u32 report_crc;

		crc = crc32_le(0xFFFFFFFF, &bthdr, 1);
		crc = ~crc32_le(crc, rd, DS4_INPUT_REPORT_0x11_SIZE-4);
		report_crc = get_unaligned_le32(&rd[DS4_INPUT_REPORT_0x11_SIZE-4]);
		if (crc != report_crc) {
			hid_dbg(sc->hdev, "DualShock 4 input report's CRC check failed, received crc 0x%0x != 0x%0x\n",
				report_crc, crc);
			return -EILSEQ;
		}

		dualshock4_parse_report(sc, rd, size);
	} else if ((sc->quirks & DUALSHOCK4_DONGLE) && rd[0] == 0x01 &&
			size == 64) {
		unsigned long flags;
		enum ds4_dongle_state dongle_state;

		/*
		 * In the case of a DS4 USB dongle, bit[2] of byte 31 indicates
		 * if a DS4 is actually connected (indicated by '0').
		 * For non-dongle, this bit is always 0 (connected).
		 */
		bool connected = (rd[31] & 0x04) ? false : true;

		spin_lock_irqsave(&sc->lock, flags);
		dongle_state = sc->ds4_dongle_state;
		spin_unlock_irqrestore(&sc->lock, flags);

		/*
		 * The dongle always sends input reports even when no
		 * DS4 is attached. When a DS4 is connected, we need to
		 * obtain calibration data before we can use it.
		 * The code below tracks dongle state and kicks of
		 * calibration when needed and only allows us to process
		 * input if a DS4 is actually connected.
		 */
		if (dongle_state == DONGLE_DISCONNECTED && connected) {
			hid_info(sc->hdev, "DualShock 4 USB dongle: controller connected\n");
			sony_set_leds(sc);

			spin_lock_irqsave(&sc->lock, flags);
			sc->ds4_dongle_state = DONGLE_CALIBRATING;
			spin_unlock_irqrestore(&sc->lock, flags);

			sony_schedule_work(sc, SONY_WORKER_HOTPLUG);

			/* Don't process the report since we don't have
			 * calibration data, but let hidraw have it anyway.
			 */
			return 0;
		} else if ((dongle_state == DONGLE_CONNECTED ||
			    dongle_state == DONGLE_DISABLED) && !connected) {
			hid_info(sc->hdev, "DualShock 4 USB dongle: controller disconnected\n");

			spin_lock_irqsave(&sc->lock, flags);
			sc->ds4_dongle_state = DONGLE_DISCONNECTED;
			spin_unlock_irqrestore(&sc->lock, flags);

			/* Return 0, so hidraw can get the report. */
			return 0;
		} else if (dongle_state == DONGLE_CALIBRATING ||
			   dongle_state == DONGLE_DISABLED ||
			   dongle_state == DONGLE_DISCONNECTED) {
			/* Return 0, so hidraw can get the report. */
			return 0;
		}

		dualshock4_parse_report(sc, rd, size);

	} else if ((sc->quirks & NSG_MRXU_REMOTE) && rd[0] == 0x02) {
		nsg_mrxu_parse_report(sc, rd, size);
		return 1;
	}

	if (sc->defer_initialization) {
		sc->defer_initialization = 0;
		sony_schedule_work(sc, SONY_WORKER_STATE);
	}

	return 0;
}