 */
static int ql_get_seg_count(struct ql3_adapter *qdev, unsigned short frags)
{
	if (qdev->device_id == QL3022_DEVICE_ID)
		return 1;

	if (frags <= 2)
		return frags + 1;
	else if (frags <= 6)
		return frags + 2;
	else if (frags <= 10)
		return frags + 3;
	else if (frags <= 14)
		return frags + 4;
	else if (frags <= 18)
		return frags + 5;
	return -1;