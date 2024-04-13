static int ath6kl_wmi_rssi_threshold_event_rx(struct wmi *wmi, u8 *datap,
					      int len)
{
	struct wmi_rssi_threshold_event *reply;
	struct wmi_rssi_threshold_params_cmd cmd;
	struct sq_threshold_params *sq_thresh;
	enum wmi_rssi_threshold_val new_threshold;
	u8 upper_rssi_threshold, lower_rssi_threshold;
	s16 rssi;
	int ret;

	if (len < sizeof(struct wmi_rssi_threshold_event))
		return -EINVAL;

	reply = (struct wmi_rssi_threshold_event *) datap;
	new_threshold = (enum wmi_rssi_threshold_val) reply->range;
	rssi = a_sle16_to_cpu(reply->rssi);

	sq_thresh = &wmi->sq_threshld[SIGNAL_QUALITY_METRICS_RSSI];

	/*
	 * Identify the threshold breached and communicate that to the app.
	 * After that install a new set of thresholds based on the signal
	 * quality reported by the target
	 */
	if (new_threshold) {
		/* Upper threshold breached */
		if (rssi < sq_thresh->upper_threshold[0]) {
			ath6kl_dbg(ATH6KL_DBG_WMI,
				   "spurious upper rssi threshold event: %d\n",
				   rssi);
		} else if ((rssi < sq_thresh->upper_threshold[1]) &&
			   (rssi >= sq_thresh->upper_threshold[0])) {
			new_threshold = WMI_RSSI_THRESHOLD1_ABOVE;
		} else if ((rssi < sq_thresh->upper_threshold[2]) &&
			   (rssi >= sq_thresh->upper_threshold[1])) {
			new_threshold = WMI_RSSI_THRESHOLD2_ABOVE;
		} else if ((rssi < sq_thresh->upper_threshold[3]) &&
			   (rssi >= sq_thresh->upper_threshold[2])) {
			new_threshold = WMI_RSSI_THRESHOLD3_ABOVE;
		} else if ((rssi < sq_thresh->upper_threshold[4]) &&
			   (rssi >= sq_thresh->upper_threshold[3])) {
			new_threshold = WMI_RSSI_THRESHOLD4_ABOVE;
		} else if ((rssi < sq_thresh->upper_threshold[5]) &&
			   (rssi >= sq_thresh->upper_threshold[4])) {
			new_threshold = WMI_RSSI_THRESHOLD5_ABOVE;
		} else if (rssi >= sq_thresh->upper_threshold[5]) {
			new_threshold = WMI_RSSI_THRESHOLD6_ABOVE;
		}
	} else {
		/* Lower threshold breached */
		if (rssi > sq_thresh->lower_threshold[0]) {
			ath6kl_dbg(ATH6KL_DBG_WMI,
				   "spurious lower rssi threshold event: %d %d\n",
				rssi, sq_thresh->lower_threshold[0]);
		} else if ((rssi > sq_thresh->lower_threshold[1]) &&
			   (rssi <= sq_thresh->lower_threshold[0])) {
			new_threshold = WMI_RSSI_THRESHOLD6_BELOW;
		} else if ((rssi > sq_thresh->lower_threshold[2]) &&
			   (rssi <= sq_thresh->lower_threshold[1])) {
			new_threshold = WMI_RSSI_THRESHOLD5_BELOW;
		} else if ((rssi > sq_thresh->lower_threshold[3]) &&
			   (rssi <= sq_thresh->lower_threshold[2])) {
			new_threshold = WMI_RSSI_THRESHOLD4_BELOW;
		} else if ((rssi > sq_thresh->lower_threshold[4]) &&
			   (rssi <= sq_thresh->lower_threshold[3])) {
			new_threshold = WMI_RSSI_THRESHOLD3_BELOW;
		} else if ((rssi > sq_thresh->lower_threshold[5]) &&
			   (rssi <= sq_thresh->lower_threshold[4])) {
			new_threshold = WMI_RSSI_THRESHOLD2_BELOW;
		} else if (rssi <= sq_thresh->lower_threshold[5]) {
			new_threshold = WMI_RSSI_THRESHOLD1_BELOW;
		}
	}

	/* Calculate and install the next set of thresholds */
	lower_rssi_threshold = ath6kl_wmi_get_lower_threshold(rssi, sq_thresh,
				       sq_thresh->lower_threshold_valid_count);
	upper_rssi_threshold = ath6kl_wmi_get_upper_threshold(rssi, sq_thresh,
				       sq_thresh->upper_threshold_valid_count);

	/* Issue a wmi command to install the thresholds */
	cmd.thresh_above1_val = a_cpu_to_sle16(upper_rssi_threshold);
	cmd.thresh_below1_val = a_cpu_to_sle16(lower_rssi_threshold);
	cmd.weight = sq_thresh->weight;
	cmd.poll_time = cpu_to_le32(sq_thresh->polling_interval);

	ret = ath6kl_wmi_send_rssi_threshold_params(wmi, &cmd);
	if (ret) {
		ath6kl_err("unable to configure rssi thresholds\n");
		return -EIO;
	}

	return 0;
}