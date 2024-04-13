static int hidp_output_report(struct hid_device *hid, __u8 *data, size_t count)
{
	struct hidp_session *session = hid->driver_data;

	return hidp_send_intr_message(session,
				      HIDP_TRANS_DATA | HIDP_DATA_RTYPE_OUPUT,
				      data, count);
}