static __u8 *ms_report_fixup(struct hid_device *hdev, __u8 *rdesc,
		unsigned int *rsize)
{
	struct ms_data *ms = hid_get_drvdata(hdev);
	unsigned long quirks = ms->quirks;

	/*
	 * Microsoft Wireless Desktop Receiver (Model 1028) has
	 * 'Usage Min/Max' where it ought to have 'Physical Min/Max'
	 */
	if ((quirks & MS_RDESC) && *rsize == 571 && rdesc[557] == 0x19 &&
			rdesc[559] == 0x29) {
		hid_info(hdev, "fixing up Microsoft Wireless Receiver Model 1028 report descriptor\n");
		rdesc[557] = 0x35;
		rdesc[559] = 0x45;
	}
	return rdesc;
}