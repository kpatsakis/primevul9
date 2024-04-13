static __u8 *dr_report_fixup(struct hid_device *hdev, __u8 *rdesc,
				unsigned int *rsize)
{
	switch (hdev->product) {
	case 0x0011:
		if (*rsize == PID0011_RDESC_ORIG_SIZE) {
			rdesc = pid0011_rdesc_fixed;
			*rsize = sizeof(pid0011_rdesc_fixed);
		}
		break;
	}
	return rdesc;
}