relpTcpGetRtryDirection(relpTcp_t *pThis)
{
	return gnutls_record_get_direction(pThis->session);
}