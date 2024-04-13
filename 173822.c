static void holtekff_send(struct holtekff_device *holtekff,
			  struct hid_device *hid,
			  const u8 data[HOLTEKFF_MSG_LENGTH])
{
	int i;

	for (i = 0; i < HOLTEKFF_MSG_LENGTH; i++) {
		holtekff->field->value[i] = data[i];
	}

	dbg_hid("sending %7ph\n", data);

	hid_hw_request(hid, holtekff->field->report, HID_REQ_SET_REPORT);
}