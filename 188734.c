i40e_status i40e_commit_partition_bw_setting(struct i40e_pf *pf)
{
	/* Commit temporary BW setting to permanent NVM image */
	enum i40e_admin_queue_err last_aq_status;
	i40e_status ret;
	u16 nvm_word;

	if (pf->hw.partition_id != 1) {
		dev_info(&pf->pdev->dev,
			 "Commit BW only works on partition 1! This is partition %d",
			 pf->hw.partition_id);
		ret = I40E_NOT_SUPPORTED;
		goto bw_commit_out;
	}

	/* Acquire NVM for read access */
	ret = i40e_acquire_nvm(&pf->hw, I40E_RESOURCE_READ);
	last_aq_status = pf->hw.aq.asq_last_status;
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Cannot acquire NVM for read access, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, last_aq_status));
		goto bw_commit_out;
	}

	/* Read word 0x10 of NVM - SW compatibility word 1 */
	ret = i40e_aq_read_nvm(&pf->hw,
			       I40E_SR_NVM_CONTROL_WORD,
			       0x10, sizeof(nvm_word), &nvm_word,
			       false, NULL);
	/* Save off last admin queue command status before releasing
	 * the NVM
	 */
	last_aq_status = pf->hw.aq.asq_last_status;
	i40e_release_nvm(&pf->hw);
	if (ret) {
		dev_info(&pf->pdev->dev, "NVM read error, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, last_aq_status));
		goto bw_commit_out;
	}

	/* Wait a bit for NVM release to complete */
	msleep(50);

	/* Acquire NVM for write access */
	ret = i40e_acquire_nvm(&pf->hw, I40E_RESOURCE_WRITE);
	last_aq_status = pf->hw.aq.asq_last_status;
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Cannot acquire NVM for write access, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, last_aq_status));
		goto bw_commit_out;
	}
	/* Write it back out unchanged to initiate update NVM,
	 * which will force a write of the shadow (alt) RAM to
	 * the NVM - thus storing the bandwidth values permanently.
	 */
	ret = i40e_aq_update_nvm(&pf->hw,
				 I40E_SR_NVM_CONTROL_WORD,
				 0x10, sizeof(nvm_word),
				 &nvm_word, true, 0, NULL);
	/* Save off last admin queue command status before releasing
	 * the NVM
	 */
	last_aq_status = pf->hw.aq.asq_last_status;
	i40e_release_nvm(&pf->hw);
	if (ret)
		dev_info(&pf->pdev->dev,
			 "BW settings NOT SAVED, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, last_aq_status));
bw_commit_out:

	return ret;
}