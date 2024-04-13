static int i40e_configure_queue_channels(struct i40e_vsi *vsi)
{
	struct i40e_channel *ch;
	u64 max_rate = 0;
	int ret = 0, i;

	/* Create app vsi with the TCs. Main VSI with TC0 is already set up */
	vsi->tc_seid_map[0] = vsi->seid;
	for (i = 1; i < I40E_MAX_TRAFFIC_CLASS; i++) {
		if (vsi->tc_config.enabled_tc & BIT(i)) {
			ch = kzalloc(sizeof(*ch), GFP_KERNEL);
			if (!ch) {
				ret = -ENOMEM;
				goto err_free;
			}

			INIT_LIST_HEAD(&ch->list);
			ch->num_queue_pairs =
				vsi->tc_config.tc_info[i].qcount;
			ch->base_queue =
				vsi->tc_config.tc_info[i].qoffset;

			/* Bandwidth limit through tc interface is in bytes/s,
			 * change to Mbit/s
			 */
			max_rate = vsi->mqprio_qopt.max_rate[i];
			do_div(max_rate, I40E_BW_MBPS_DIVISOR);
			ch->max_tx_rate = max_rate;

			list_add_tail(&ch->list, &vsi->ch_list);

			ret = i40e_create_queue_channel(vsi, ch);
			if (ret) {
				dev_err(&vsi->back->pdev->dev,
					"Failed creating queue channel with TC%d: queues %d\n",
					i, ch->num_queue_pairs);
				goto err_free;
			}
			vsi->tc_seid_map[i] = ch->seid;
		}
	}
	return ret;

err_free:
	i40e_remove_queue_channels(vsi);
	return ret;
}