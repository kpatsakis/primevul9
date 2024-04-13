static int hidp_raw_request(struct hid_device *hid, unsigned char reportnum,
			    __u8 *buf, size_t len, unsigned char rtype,
			    int reqtype)
{
	switch (reqtype) {
	case HID_REQ_GET_REPORT:
		return hidp_get_raw_report(hid, reportnum, buf, len, rtype);
	case HID_REQ_SET_REPORT:
		return hidp_set_raw_report(hid, reportnum, buf, len, rtype);
	default:
		return -EIO;
	}
}