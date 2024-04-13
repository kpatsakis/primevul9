static void i40e_clean_adminq_subtask(struct i40e_pf *pf)
{
	struct i40e_arq_event_info event;
	struct i40e_hw *hw = &pf->hw;
	u16 pending, i = 0;
	i40e_status ret;
	u16 opcode;
	u32 oldval;
	u32 val;

	/* Do not run clean AQ when PF reset fails */
	if (test_bit(__I40E_RESET_FAILED, pf->state))
		return;

	/* check for error indications */
	val = rd32(&pf->hw, pf->hw.aq.arq.len);
	oldval = val;
	if (val & I40E_PF_ARQLEN_ARQVFE_MASK) {
		if (hw->debug_mask & I40E_DEBUG_AQ)
			dev_info(&pf->pdev->dev, "ARQ VF Error detected\n");
		val &= ~I40E_PF_ARQLEN_ARQVFE_MASK;
	}
	if (val & I40E_PF_ARQLEN_ARQOVFL_MASK) {
		if (hw->debug_mask & I40E_DEBUG_AQ)
			dev_info(&pf->pdev->dev, "ARQ Overflow Error detected\n");
		val &= ~I40E_PF_ARQLEN_ARQOVFL_MASK;
		pf->arq_overflows++;
	}
	if (val & I40E_PF_ARQLEN_ARQCRIT_MASK) {
		if (hw->debug_mask & I40E_DEBUG_AQ)
			dev_info(&pf->pdev->dev, "ARQ Critical Error detected\n");
		val &= ~I40E_PF_ARQLEN_ARQCRIT_MASK;
	}
	if (oldval != val)
		wr32(&pf->hw, pf->hw.aq.arq.len, val);

	val = rd32(&pf->hw, pf->hw.aq.asq.len);
	oldval = val;
	if (val & I40E_PF_ATQLEN_ATQVFE_MASK) {
		if (pf->hw.debug_mask & I40E_DEBUG_AQ)
			dev_info(&pf->pdev->dev, "ASQ VF Error detected\n");
		val &= ~I40E_PF_ATQLEN_ATQVFE_MASK;
	}
	if (val & I40E_PF_ATQLEN_ATQOVFL_MASK) {
		if (pf->hw.debug_mask & I40E_DEBUG_AQ)
			dev_info(&pf->pdev->dev, "ASQ Overflow Error detected\n");
		val &= ~I40E_PF_ATQLEN_ATQOVFL_MASK;
	}
	if (val & I40E_PF_ATQLEN_ATQCRIT_MASK) {
		if (pf->hw.debug_mask & I40E_DEBUG_AQ)
			dev_info(&pf->pdev->dev, "ASQ Critical Error detected\n");
		val &= ~I40E_PF_ATQLEN_ATQCRIT_MASK;
	}
	if (oldval != val)
		wr32(&pf->hw, pf->hw.aq.asq.len, val);

	event.buf_len = I40E_MAX_AQ_BUF_SIZE;
	event.msg_buf = kzalloc(event.buf_len, GFP_KERNEL);
	if (!event.msg_buf)
		return;

	do {
		ret = i40e_clean_arq_element(hw, &event, &pending);
		if (ret == I40E_ERR_ADMIN_QUEUE_NO_WORK)
			break;
		else if (ret) {
			dev_info(&pf->pdev->dev, "ARQ event error %d\n", ret);
			break;
		}

		opcode = le16_to_cpu(event.desc.opcode);
		switch (opcode) {

		case i40e_aqc_opc_get_link_status:
			i40e_handle_link_event(pf, &event);
			break;
		case i40e_aqc_opc_send_msg_to_pf:
			ret = i40e_vc_process_vf_msg(pf,
					le16_to_cpu(event.desc.retval),
					le32_to_cpu(event.desc.cookie_high),
					le32_to_cpu(event.desc.cookie_low),
					event.msg_buf,
					event.msg_len);
			break;
		case i40e_aqc_opc_lldp_update_mib:
			dev_dbg(&pf->pdev->dev, "ARQ: Update LLDP MIB event received\n");
#ifdef CONFIG_I40E_DCB
			rtnl_lock();
			ret = i40e_handle_lldp_event(pf, &event);
			rtnl_unlock();
#endif /* CONFIG_I40E_DCB */
			break;
		case i40e_aqc_opc_event_lan_overflow:
			dev_dbg(&pf->pdev->dev, "ARQ LAN queue overflow event received\n");
			i40e_handle_lan_overflow_event(pf, &event);
			break;
		case i40e_aqc_opc_send_msg_to_peer:
			dev_info(&pf->pdev->dev, "ARQ: Msg from other pf\n");
			break;
		case i40e_aqc_opc_nvm_erase:
		case i40e_aqc_opc_nvm_update:
		case i40e_aqc_opc_oem_post_update:
			i40e_debug(&pf->hw, I40E_DEBUG_NVM,
				   "ARQ NVM operation 0x%04x completed\n",
				   opcode);
			break;
		default:
			dev_info(&pf->pdev->dev,
				 "ARQ: Unknown event 0x%04x ignored\n",
				 opcode);
			break;
		}
	} while (i++ < pf->adminq_work_limit);

	if (i < pf->adminq_work_limit)
		clear_bit(__I40E_ADMINQ_EVENT_PENDING, pf->state);

	/* re-enable Admin queue interrupt cause */
	val = rd32(hw, I40E_PFINT_ICR0_ENA);
	val |=  I40E_PFINT_ICR0_ENA_ADMINQ_MASK;
	wr32(hw, I40E_PFINT_ICR0_ENA, val);
	i40e_flush(hw);

	kfree(event.msg_buf);
}