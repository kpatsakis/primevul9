rdpShadowServer* shadow_server_new(void)
{
	rdpShadowServer* server;
	server = (rdpShadowServer*)calloc(1, sizeof(rdpShadowServer));

	if (!server)
		return NULL;

	server->port = 3389;
	server->mayView = TRUE;
	server->mayInteract = TRUE;
	server->rfxMode = RLGR3;
	server->h264RateControlMode = H264_RATECONTROL_VBR;
	server->h264BitRate = 10000000;
	server->h264FrameRate = 30;
	server->h264QP = 0;
	server->authentication = FALSE;
	server->settings = freerdp_settings_new(FREERDP_SETTINGS_SERVER_MODE);
	return server;
}