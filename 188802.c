static int i40e_validate_mqprio_qopt(struct i40e_vsi *vsi,
				     struct tc_mqprio_qopt_offload *mqprio_qopt)
{
	u64 sum_max_rate = 0;
	u64 max_rate = 0;
	int i;

	if (mqprio_qopt->qopt.offset[0] != 0 ||
	    mqprio_qopt->qopt.num_tc < 1 ||
	    mqprio_qopt->qopt.num_tc > I40E_MAX_TRAFFIC_CLASS)
		return -EINVAL;
	for (i = 0; ; i++) {
		if (!mqprio_qopt->qopt.count[i])
			return -EINVAL;
		if (mqprio_qopt->min_rate[i]) {
			dev_err(&vsi->back->pdev->dev,
				"Invalid min tx rate (greater than 0) specified\n");
			return -EINVAL;
		}
		max_rate = mqprio_qopt->max_rate[i];
		do_div(max_rate, I40E_BW_MBPS_DIVISOR);
		sum_max_rate += max_rate;

		if (i >= mqprio_qopt->qopt.num_tc - 1)
			break;
		if (mqprio_qopt->qopt.offset[i + 1] !=
		    (mqprio_qopt->qopt.offset[i] + mqprio_qopt->qopt.count[i]))
			return -EINVAL;
	}
	if (vsi->num_queue_pairs <
	    (mqprio_qopt->qopt.offset[i] + mqprio_qopt->qopt.count[i])) {
		return -EINVAL;
	}
	if (sum_max_rate > i40e_get_link_speed(vsi)) {
		dev_err(&vsi->back->pdev->dev,
			"Invalid max tx rate specified\n");
		return -EINVAL;
	}
	return 0;
}