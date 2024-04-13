static u8 *ps3remote_fixup(struct hid_device *hdev, u8 *rdesc,
			     unsigned int *rsize)
{
	*rsize = sizeof(ps3remote_rdesc);
	return ps3remote_rdesc;
}