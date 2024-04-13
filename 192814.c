void hclge_tm_schd_info_update(struct hclge_dev *hdev, u8 num_tc)
{
	u8 bit_map = 0;
	u8 i;

	hdev->tm_info.num_tc = num_tc;

	for (i = 0; i < hdev->tm_info.num_tc; i++)
		bit_map |= BIT(i);

	if (!bit_map) {
		bit_map = 1;
		hdev->tm_info.num_tc = 1;
	}

	hdev->hw_tc_map = bit_map;

	hclge_tm_schd_info_init(hdev);
}