static int i40e_resume_port_tx(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	int ret;

	ret = i40e_aq_resume_port_tx(hw, NULL);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "Resume Port Tx failed, err %s aq_err %s\n",
			  i40e_stat_str(&pf->hw, ret),
			  i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		/* Schedule PF reset to recover */
		set_bit(__I40E_PF_RESET_REQUESTED, pf->state);
		i40e_service_event_schedule(pf);
	}

	return ret;
}