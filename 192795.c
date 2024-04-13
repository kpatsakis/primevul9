void hclge_tm_pfc_info_update(struct hclge_dev *hdev)
{
	/* DCB is enabled if we have more than 1 TC or pfc_en is
	 * non-zero.
	 */
	if (hdev->tm_info.num_tc > 1 || hdev->tm_info.pfc_en)
		hdev->flag |= HCLGE_FLAG_DCB_ENABLE;
	else
		hdev->flag &= ~HCLGE_FLAG_DCB_ENABLE;

	hclge_pfc_info_init(hdev);
}