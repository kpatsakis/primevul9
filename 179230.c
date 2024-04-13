int rndis_signal_disconnect(struct rndis_params *params)
{
	params->media_state = RNDIS_MEDIA_STATE_DISCONNECTED;
	return rndis_indicate_status_msg(params, RNDIS_STATUS_MEDIA_DISCONNECT);
}