static void hclge_pfc_info_init(struct hclge_dev *hdev)
{
	if (!(hdev->flag & HCLGE_FLAG_DCB_ENABLE)) {
		if (hdev->fc_mode_last_time == HCLGE_FC_PFC)
			dev_warn(&hdev->pdev->dev,
				 "DCB is disable, but last mode is FC_PFC\n");

		hdev->tm_info.fc_mode = hdev->fc_mode_last_time;
	} else if (hdev->tm_info.fc_mode != HCLGE_FC_PFC) {
		/* fc_mode_last_time record the last fc_mode when
		 * DCB is enabled, so that fc_mode can be set to
		 * the correct value when DCB is disabled.
		 */
		hdev->fc_mode_last_time = hdev->tm_info.fc_mode;
		hdev->tm_info.fc_mode = HCLGE_FC_PFC;
	}
}