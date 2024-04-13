static void i40e_handle_mdd_event(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	bool mdd_detected = false;
	struct i40e_vf *vf;
	u32 reg;
	int i;

	if (!test_bit(__I40E_MDD_EVENT_PENDING, pf->state))
		return;

	/* find what triggered the MDD event */
	reg = rd32(hw, I40E_GL_MDET_TX);
	if (reg & I40E_GL_MDET_TX_VALID_MASK) {
		u8 pf_num = (reg & I40E_GL_MDET_TX_PF_NUM_MASK) >>
				I40E_GL_MDET_TX_PF_NUM_SHIFT;
		u16 vf_num = (reg & I40E_GL_MDET_TX_VF_NUM_MASK) >>
				I40E_GL_MDET_TX_VF_NUM_SHIFT;
		u8 event = (reg & I40E_GL_MDET_TX_EVENT_MASK) >>
				I40E_GL_MDET_TX_EVENT_SHIFT;
		u16 queue = ((reg & I40E_GL_MDET_TX_QUEUE_MASK) >>
				I40E_GL_MDET_TX_QUEUE_SHIFT) -
				pf->hw.func_caps.base_queue;
		if (netif_msg_tx_err(pf))
			dev_info(&pf->pdev->dev, "Malicious Driver Detection event 0x%02x on TX queue %d PF number 0x%02x VF number 0x%02x\n",
				 event, queue, pf_num, vf_num);
		wr32(hw, I40E_GL_MDET_TX, 0xffffffff);
		mdd_detected = true;
	}
	reg = rd32(hw, I40E_GL_MDET_RX);
	if (reg & I40E_GL_MDET_RX_VALID_MASK) {
		u8 func = (reg & I40E_GL_MDET_RX_FUNCTION_MASK) >>
				I40E_GL_MDET_RX_FUNCTION_SHIFT;
		u8 event = (reg & I40E_GL_MDET_RX_EVENT_MASK) >>
				I40E_GL_MDET_RX_EVENT_SHIFT;
		u16 queue = ((reg & I40E_GL_MDET_RX_QUEUE_MASK) >>
				I40E_GL_MDET_RX_QUEUE_SHIFT) -
				pf->hw.func_caps.base_queue;
		if (netif_msg_rx_err(pf))
			dev_info(&pf->pdev->dev, "Malicious Driver Detection event 0x%02x on RX queue %d of function 0x%02x\n",
				 event, queue, func);
		wr32(hw, I40E_GL_MDET_RX, 0xffffffff);
		mdd_detected = true;
	}

	if (mdd_detected) {
		reg = rd32(hw, I40E_PF_MDET_TX);
		if (reg & I40E_PF_MDET_TX_VALID_MASK) {
			wr32(hw, I40E_PF_MDET_TX, 0xFFFF);
			dev_dbg(&pf->pdev->dev, "TX driver issue detected on PF\n");
		}
		reg = rd32(hw, I40E_PF_MDET_RX);
		if (reg & I40E_PF_MDET_RX_VALID_MASK) {
			wr32(hw, I40E_PF_MDET_RX, 0xFFFF);
			dev_dbg(&pf->pdev->dev, "RX driver issue detected on PF\n");
		}
	}

	/* see if one of the VFs needs its hand slapped */
	for (i = 0; i < pf->num_alloc_vfs && mdd_detected; i++) {
		vf = &(pf->vf[i]);
		reg = rd32(hw, I40E_VP_MDET_TX(i));
		if (reg & I40E_VP_MDET_TX_VALID_MASK) {
			wr32(hw, I40E_VP_MDET_TX(i), 0xFFFF);
			vf->num_mdd_events++;
			dev_info(&pf->pdev->dev, "TX driver issue detected on VF %d\n",
				 i);
			dev_info(&pf->pdev->dev,
				 "Use PF Control I/F to re-enable the VF\n");
			set_bit(I40E_VF_STATE_DISABLED, &vf->vf_states);
		}

		reg = rd32(hw, I40E_VP_MDET_RX(i));
		if (reg & I40E_VP_MDET_RX_VALID_MASK) {
			wr32(hw, I40E_VP_MDET_RX(i), 0xFFFF);
			vf->num_mdd_events++;
			dev_info(&pf->pdev->dev, "RX driver issue detected on VF %d\n",
				 i);
			dev_info(&pf->pdev->dev,
				 "Use PF Control I/F to re-enable the VF\n");
			set_bit(I40E_VF_STATE_DISABLED, &vf->vf_states);
		}
	}

	/* re-enable mdd interrupt cause */
	clear_bit(__I40E_MDD_EVENT_PENDING, pf->state);
	reg = rd32(hw, I40E_PFINT_ICR0_ENA);
	reg |=  I40E_PFINT_ICR0_ENA_MAL_DETECT_MASK;
	wr32(hw, I40E_PFINT_ICR0_ENA, reg);
	i40e_flush(hw);
}