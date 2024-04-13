static int i40e_validate_num_queues(struct i40e_pf *pf, int num_queues,
				    struct i40e_vsi *vsi, bool *reconfig_rss)
{
	int max_ch_queues;

	if (!reconfig_rss)
		return -EINVAL;

	*reconfig_rss = false;
	if (vsi->current_rss_size) {
		if (num_queues > vsi->current_rss_size) {
			dev_dbg(&pf->pdev->dev,
				"Error: num_queues (%d) > vsi's current_size(%d)\n",
				num_queues, vsi->current_rss_size);
			return -EINVAL;
		} else if ((num_queues < vsi->current_rss_size) &&
			   (!is_power_of_2(num_queues))) {
			dev_dbg(&pf->pdev->dev,
				"Error: num_queues (%d) < vsi's current_size(%d), but not power of 2\n",
				num_queues, vsi->current_rss_size);
			return -EINVAL;
		}
	}

	if (!is_power_of_2(num_queues)) {
		/* Find the max num_queues configured for channel if channel
		 * exist.
		 * if channel exist, then enforce 'num_queues' to be more than
		 * max ever queues configured for channel.
		 */
		max_ch_queues = i40e_get_max_queues_for_channel(vsi);
		if (num_queues < max_ch_queues) {
			dev_dbg(&pf->pdev->dev,
				"Error: num_queues (%d) < max queues configured for channel(%d)\n",
				num_queues, max_ch_queues);
			return -EINVAL;
		}
		*reconfig_rss = true;
	}

	return 0;
}