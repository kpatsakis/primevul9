static int i40e_get_capabilities(struct i40e_pf *pf,
				 enum i40e_admin_queue_opc list_type)
{
	struct i40e_aqc_list_capabilities_element_resp *cap_buf;
	u16 data_size;
	int buf_len;
	int err;

	buf_len = 40 * sizeof(struct i40e_aqc_list_capabilities_element_resp);
	do {
		cap_buf = kzalloc(buf_len, GFP_KERNEL);
		if (!cap_buf)
			return -ENOMEM;

		/* this loads the data into the hw struct for us */
		err = i40e_aq_discover_capabilities(&pf->hw, cap_buf, buf_len,
						    &data_size, list_type,
						    NULL);
		/* data loaded, buffer no longer needed */
		kfree(cap_buf);

		if (pf->hw.aq.asq_last_status == I40E_AQ_RC_ENOMEM) {
			/* retry with a larger buffer */
			buf_len = data_size;
		} else if (pf->hw.aq.asq_last_status != I40E_AQ_RC_OK) {
			dev_info(&pf->pdev->dev,
				 "capability discovery failed, err %s aq_err %s\n",
				 i40e_stat_str(&pf->hw, err),
				 i40e_aq_str(&pf->hw,
					     pf->hw.aq.asq_last_status));
			return -ENODEV;
		}
	} while (err);

	if (pf->hw.debug_mask & I40E_DEBUG_USER) {
		if (list_type == i40e_aqc_opc_list_func_capabilities) {
			dev_info(&pf->pdev->dev,
				 "pf=%d, num_vfs=%d, msix_pf=%d, msix_vf=%d, fd_g=%d, fd_b=%d, pf_max_q=%d num_vsi=%d\n",
				 pf->hw.pf_id, pf->hw.func_caps.num_vfs,
				 pf->hw.func_caps.num_msix_vectors,
				 pf->hw.func_caps.num_msix_vectors_vf,
				 pf->hw.func_caps.fd_filters_guaranteed,
				 pf->hw.func_caps.fd_filters_best_effort,
				 pf->hw.func_caps.num_tx_qp,
				 pf->hw.func_caps.num_vsis);
		} else if (list_type == i40e_aqc_opc_list_dev_capabilities) {
			dev_info(&pf->pdev->dev,
				 "switch_mode=0x%04x, function_valid=0x%08x\n",
				 pf->hw.dev_caps.switch_mode,
				 pf->hw.dev_caps.valid_functions);
			dev_info(&pf->pdev->dev,
				 "SR-IOV=%d, num_vfs for all function=%u\n",
				 pf->hw.dev_caps.sr_iov_1_1,
				 pf->hw.dev_caps.num_vfs);
			dev_info(&pf->pdev->dev,
				 "num_vsis=%u, num_rx:%u, num_tx=%u\n",
				 pf->hw.dev_caps.num_vsis,
				 pf->hw.dev_caps.num_rx_qp,
				 pf->hw.dev_caps.num_tx_qp);
		}
	}
	if (list_type == i40e_aqc_opc_list_func_capabilities) {
#define DEF_NUM_VSI (1 + (pf->hw.func_caps.fcoe ? 1 : 0) \
		       + pf->hw.func_caps.num_vfs)
		if (pf->hw.revision_id == 0 &&
		    pf->hw.func_caps.num_vsis < DEF_NUM_VSI) {
			dev_info(&pf->pdev->dev,
				 "got num_vsis %d, setting num_vsis to %d\n",
				 pf->hw.func_caps.num_vsis, DEF_NUM_VSI);
			pf->hw.func_caps.num_vsis = DEF_NUM_VSI;
		}
	}
	return 0;
}