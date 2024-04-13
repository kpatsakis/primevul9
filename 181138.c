static int ath6kl_wmi_delba_req_event_rx(struct wmi *wmi, u8 *datap, int len,
					 struct ath6kl_vif *vif)
{
	struct wmi_delba_event *cmd = (struct wmi_delba_event *) datap;

	aggr_recv_delba_req_evt(vif, cmd->tid);

	return 0;
}