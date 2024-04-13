static int dualshock4_get_calibration_data(struct sony_sc *sc)
{
	u8 *buf;
	int ret;
	short gyro_pitch_bias, gyro_pitch_plus, gyro_pitch_minus;
	short gyro_yaw_bias, gyro_yaw_plus, gyro_yaw_minus;
	short gyro_roll_bias, gyro_roll_plus, gyro_roll_minus;
	short gyro_speed_plus, gyro_speed_minus;
	short acc_x_plus, acc_x_minus;
	short acc_y_plus, acc_y_minus;
	short acc_z_plus, acc_z_minus;
	int speed_2x;
	int range_2g;

	/* For Bluetooth we use a different request, which supports CRC.
	 * Note: in Bluetooth mode feature report 0x02 also changes the state
	 * of the controller, so that it sends input reports of type 0x11.
	 */
	if (sc->quirks & (DUALSHOCK4_CONTROLLER_USB | DUALSHOCK4_DONGLE)) {
		buf = kmalloc(DS4_FEATURE_REPORT_0x02_SIZE, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		ret = hid_hw_raw_request(sc->hdev, 0x02, buf,
					 DS4_FEATURE_REPORT_0x02_SIZE,
					 HID_FEATURE_REPORT,
					 HID_REQ_GET_REPORT);
		if (ret < 0)
			goto err_stop;
	} else {
		u8 bthdr = 0xA3;
		u32 crc;
		u32 report_crc;
		int retries;

		buf = kmalloc(DS4_FEATURE_REPORT_0x05_SIZE, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		for (retries = 0; retries < 3; retries++) {
			ret = hid_hw_raw_request(sc->hdev, 0x05, buf,
						 DS4_FEATURE_REPORT_0x05_SIZE,
						 HID_FEATURE_REPORT,
						 HID_REQ_GET_REPORT);
			if (ret < 0)
				goto err_stop;

			/* CRC check */
			crc = crc32_le(0xFFFFFFFF, &bthdr, 1);
			crc = ~crc32_le(crc, buf, DS4_FEATURE_REPORT_0x05_SIZE-4);
			report_crc = get_unaligned_le32(&buf[DS4_FEATURE_REPORT_0x05_SIZE-4]);
			if (crc != report_crc) {
				hid_warn(sc->hdev, "DualShock 4 calibration report's CRC check failed, received crc 0x%0x != 0x%0x\n",
					report_crc, crc);
				if (retries < 2) {
					hid_warn(sc->hdev, "Retrying DualShock 4 get calibration report request\n");
					continue;
				} else {
					ret = -EILSEQ;
					goto err_stop;
				}
			} else {
				break;
			}
		}
	}

	gyro_pitch_bias  = get_unaligned_le16(&buf[1]);
	gyro_yaw_bias    = get_unaligned_le16(&buf[3]);
	gyro_roll_bias   = get_unaligned_le16(&buf[5]);
	if (sc->quirks & DUALSHOCK4_CONTROLLER_USB) {
		gyro_pitch_plus  = get_unaligned_le16(&buf[7]);
		gyro_pitch_minus = get_unaligned_le16(&buf[9]);
		gyro_yaw_plus    = get_unaligned_le16(&buf[11]);
		gyro_yaw_minus   = get_unaligned_le16(&buf[13]);
		gyro_roll_plus   = get_unaligned_le16(&buf[15]);
		gyro_roll_minus  = get_unaligned_le16(&buf[17]);
	} else {
		/* BT + Dongle */
		gyro_pitch_plus  = get_unaligned_le16(&buf[7]);
		gyro_yaw_plus    = get_unaligned_le16(&buf[9]);
		gyro_roll_plus   = get_unaligned_le16(&buf[11]);
		gyro_pitch_minus = get_unaligned_le16(&buf[13]);
		gyro_yaw_minus   = get_unaligned_le16(&buf[15]);
		gyro_roll_minus  = get_unaligned_le16(&buf[17]);
	}
	gyro_speed_plus  = get_unaligned_le16(&buf[19]);
	gyro_speed_minus = get_unaligned_le16(&buf[21]);
	acc_x_plus       = get_unaligned_le16(&buf[23]);
	acc_x_minus      = get_unaligned_le16(&buf[25]);
	acc_y_plus       = get_unaligned_le16(&buf[27]);
	acc_y_minus      = get_unaligned_le16(&buf[29]);
	acc_z_plus       = get_unaligned_le16(&buf[31]);
	acc_z_minus      = get_unaligned_le16(&buf[33]);

	/* Set gyroscope calibration and normalization parameters.
	 * Data values will be normalized to 1/DS4_GYRO_RES_PER_DEG_S degree/s.
	 */
	speed_2x = (gyro_speed_plus + gyro_speed_minus);
	sc->ds4_calib_data[0].abs_code = ABS_RX;
	sc->ds4_calib_data[0].bias = gyro_pitch_bias;
	sc->ds4_calib_data[0].sens_numer = speed_2x*DS4_GYRO_RES_PER_DEG_S;
	sc->ds4_calib_data[0].sens_denom = gyro_pitch_plus - gyro_pitch_minus;

	sc->ds4_calib_data[1].abs_code = ABS_RY;
	sc->ds4_calib_data[1].bias = gyro_yaw_bias;
	sc->ds4_calib_data[1].sens_numer = speed_2x*DS4_GYRO_RES_PER_DEG_S;
	sc->ds4_calib_data[1].sens_denom = gyro_yaw_plus - gyro_yaw_minus;

	sc->ds4_calib_data[2].abs_code = ABS_RZ;
	sc->ds4_calib_data[2].bias = gyro_roll_bias;
	sc->ds4_calib_data[2].sens_numer = speed_2x*DS4_GYRO_RES_PER_DEG_S;
	sc->ds4_calib_data[2].sens_denom = gyro_roll_plus - gyro_roll_minus;

	/* Set accelerometer calibration and normalization parameters.
	 * Data values will be normalized to 1/DS4_ACC_RES_PER_G G.
	 */
	range_2g = acc_x_plus - acc_x_minus;
	sc->ds4_calib_data[3].abs_code = ABS_X;
	sc->ds4_calib_data[3].bias = acc_x_plus - range_2g / 2;
	sc->ds4_calib_data[3].sens_numer = 2*DS4_ACC_RES_PER_G;
	sc->ds4_calib_data[3].sens_denom = range_2g;

	range_2g = acc_y_plus - acc_y_minus;
	sc->ds4_calib_data[4].abs_code = ABS_Y;
	sc->ds4_calib_data[4].bias = acc_y_plus - range_2g / 2;
	sc->ds4_calib_data[4].sens_numer = 2*DS4_ACC_RES_PER_G;
	sc->ds4_calib_data[4].sens_denom = range_2g;

	range_2g = acc_z_plus - acc_z_minus;
	sc->ds4_calib_data[5].abs_code = ABS_Z;
	sc->ds4_calib_data[5].bias = acc_z_plus - range_2g / 2;
	sc->ds4_calib_data[5].sens_numer = 2*DS4_ACC_RES_PER_G;
	sc->ds4_calib_data[5].sens_denom = range_2g;

err_stop:
	kfree(buf);
	return ret;
}