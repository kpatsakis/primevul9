static int ath6kl_wmi_addba_req_event_rx(struct wmi *wmi, u8 *datap, int len,
					 struct ath6kl_vif *vif)
{
	struct wmi_addba_req_event *cmd = (struct wmi_addba_req_event *) datap;

	aggr_recv_addba_req_evt(vif, cmd->tid,
				le16_to_cpu(cmd->st_seq_no), cmd->win_sz);

	return 0;
}