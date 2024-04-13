static int ath6kl_wmi_tx_pwr_reply_rx(struct wmi *wmi, u8 *datap, int len)
{
	struct wmi_tx_pwr_reply *reply;

	if (len < sizeof(struct wmi_tx_pwr_reply))
		return -EINVAL;

	reply = (struct wmi_tx_pwr_reply *) datap;
	ath6kl_txpwr_rx_evt(wmi->parent_dev, reply->dbM);

	return 0;
}