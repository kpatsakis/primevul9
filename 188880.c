static int i40e_setup_tc(struct net_device *netdev, void *type_data)
{
	struct tc_mqprio_qopt_offload *mqprio_qopt = type_data;
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	u8 enabled_tc = 0, num_tc, hw;
	bool need_reset = false;
	int old_queue_pairs;
	int ret = -EINVAL;
	u16 mode;
	int i;

	old_queue_pairs = vsi->num_queue_pairs;
	num_tc = mqprio_qopt->qopt.num_tc;
	hw = mqprio_qopt->qopt.hw;
	mode = mqprio_qopt->mode;
	if (!hw) {
		pf->flags &= ~I40E_FLAG_TC_MQPRIO;
		memcpy(&vsi->mqprio_qopt, mqprio_qopt, sizeof(*mqprio_qopt));
		goto config_tc;
	}

	/* Check if MFP enabled */
	if (pf->flags & I40E_FLAG_MFP_ENABLED) {
		netdev_info(netdev,
			    "Configuring TC not supported in MFP mode\n");
		return ret;
	}
	switch (mode) {
	case TC_MQPRIO_MODE_DCB:
		pf->flags &= ~I40E_FLAG_TC_MQPRIO;

		/* Check if DCB enabled to continue */
		if (!(pf->flags & I40E_FLAG_DCB_ENABLED)) {
			netdev_info(netdev,
				    "DCB is not enabled for adapter\n");
			return ret;
		}

		/* Check whether tc count is within enabled limit */
		if (num_tc > i40e_pf_get_num_tc(pf)) {
			netdev_info(netdev,
				    "TC count greater than enabled on link for adapter\n");
			return ret;
		}
		break;
	case TC_MQPRIO_MODE_CHANNEL:
		if (pf->flags & I40E_FLAG_DCB_ENABLED) {
			netdev_info(netdev,
				    "Full offload of TC Mqprio options is not supported when DCB is enabled\n");
			return ret;
		}
		if (!(pf->flags & I40E_FLAG_MSIX_ENABLED))
			return ret;
		ret = i40e_validate_mqprio_qopt(vsi, mqprio_qopt);
		if (ret)
			return ret;
		memcpy(&vsi->mqprio_qopt, mqprio_qopt,
		       sizeof(*mqprio_qopt));
		pf->flags |= I40E_FLAG_TC_MQPRIO;
		pf->flags &= ~I40E_FLAG_DCB_ENABLED;
		break;
	default:
		return -EINVAL;
	}

config_tc:
	/* Generate TC map for number of tc requested */
	for (i = 0; i < num_tc; i++)
		enabled_tc |= BIT(i);

	/* Requesting same TC configuration as already enabled */
	if (enabled_tc == vsi->tc_config.enabled_tc &&
	    mode != TC_MQPRIO_MODE_CHANNEL)
		return 0;

	/* Quiesce VSI queues */
	i40e_quiesce_vsi(vsi);

	if (!hw && !(pf->flags & I40E_FLAG_TC_MQPRIO))
		i40e_remove_queue_channels(vsi);

	/* Configure VSI for enabled TCs */
	ret = i40e_vsi_config_tc(vsi, enabled_tc);
	if (ret) {
		netdev_info(netdev, "Failed configuring TC for VSI seid=%d\n",
			    vsi->seid);
		need_reset = true;
		goto exit;
	} else {
		dev_info(&vsi->back->pdev->dev,
			 "Setup channel (id:%u) utilizing num_queues %d\n",
			 vsi->seid, vsi->tc_config.tc_info[0].qcount);
	}

	if (pf->flags & I40E_FLAG_TC_MQPRIO) {
		if (vsi->mqprio_qopt.max_rate[0]) {
			u64 max_tx_rate = vsi->mqprio_qopt.max_rate[0];

			do_div(max_tx_rate, I40E_BW_MBPS_DIVISOR);
			ret = i40e_set_bw_limit(vsi, vsi->seid, max_tx_rate);
			if (!ret) {
				u64 credits = max_tx_rate;

				do_div(credits, I40E_BW_CREDIT_DIVISOR);
				dev_dbg(&vsi->back->pdev->dev,
					"Set tx rate of %llu Mbps (count of 50Mbps %llu) for vsi->seid %u\n",
					max_tx_rate,
					credits,
					vsi->seid);
			} else {
				need_reset = true;
				goto exit;
			}
		}
		ret = i40e_configure_queue_channels(vsi);
		if (ret) {
			vsi->num_queue_pairs = old_queue_pairs;
			netdev_info(netdev,
				    "Failed configuring queue channels\n");
			need_reset = true;
			goto exit;
		}
	}

exit:
	/* Reset the configuration data to defaults, only TC0 is enabled */
	if (need_reset) {
		i40e_vsi_set_default_tc_config(vsi);
		need_reset = false;
	}

	/* Unquiesce VSI */
	i40e_unquiesce_vsi(vsi);
	return ret;
}