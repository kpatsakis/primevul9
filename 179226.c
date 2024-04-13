int rndis_signal_connect(struct rndis_params *params)
{
	params->media_state = RNDIS_MEDIA_STATE_CONNECTED;
	return rndis_indicate_status_msg(params, RNDIS_STATUS_MEDIA_CONNECT);
}