static void hidpp_touchpad_raw_xy_event(struct hidpp_device *hidpp_dev,
		u8 *data, struct hidpp_touchpad_raw_xy *raw_xy)
{
	memset(raw_xy, 0, sizeof(struct hidpp_touchpad_raw_xy));
	raw_xy->end_of_frame = data[8] & 0x01;
	raw_xy->spurious_flag = (data[8] >> 1) & 0x01;
	raw_xy->finger_count = data[15] & 0x0f;
	raw_xy->button = (data[8] >> 2) & 0x01;

	if (raw_xy->finger_count) {
		hidpp_touchpad_touch_event(&data[2], &raw_xy->fingers[0]);
		hidpp_touchpad_touch_event(&data[9], &raw_xy->fingers[1]);
	}
}