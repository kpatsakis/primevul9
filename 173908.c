static int wtp_mouse_raw_xy_event(struct hidpp_device *hidpp, u8 *data)
{
	struct wtp_data *wd = hidpp->private_data;
	u8 c1_area = ((data[7] & 0xf) * (data[7] & 0xf) +
		      (data[7] >> 4) * (data[7] >> 4)) / 2;
	u8 c2_area = ((data[13] & 0xf) * (data[13] & 0xf) +
		      (data[13] >> 4) * (data[13] >> 4)) / 2;
	struct hidpp_touchpad_raw_xy raw = {
		.timestamp = data[1],
		.fingers = {
			{
				.contact_type = 0,
				.contact_status = !!data[7],
				.x = get_unaligned_le16(&data[3]),
				.y = get_unaligned_le16(&data[5]),
				.z = c1_area,
				.area = c1_area,
				.finger_id = data[2],
			}, {
				.contact_type = 0,
				.contact_status = !!data[13],
				.x = get_unaligned_le16(&data[9]),
				.y = get_unaligned_le16(&data[11]),
				.z = c2_area,
				.area = c2_area,
				.finger_id = data[8],
			}
		},
		.finger_count = wd->maxcontacts,
		.spurious_flag = 0,
		.end_of_frame = (data[0] >> 7) == 0,
		.button = data[0] & 0x01,
	};

	wtp_send_raw_xy_event(hidpp, &raw);

	return 1;
}