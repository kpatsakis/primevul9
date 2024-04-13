static u8 *motion_fixup(struct hid_device *hdev, u8 *rdesc,
			     unsigned int *rsize)
{
	*rsize = sizeof(motion_rdesc);
	return motion_rdesc;
}