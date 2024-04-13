client_allocsendbuf(ns_client_t *client, isc_buffer_t *buffer,
		    isc_buffer_t *tcpbuffer, uint32_t length,
		    unsigned char *sendbuf, unsigned char **datap)
{
	unsigned char *data;
	uint32_t bufsize;
	isc_result_t result;

	INSIST(datap != NULL);
	INSIST((tcpbuffer == NULL && length != 0) ||
	       (tcpbuffer != NULL && length == 0));

	if (TCP_CLIENT(client)) {
		INSIST(client->tcpbuf == NULL);
		if (length + 2 > TCP_BUFFER_SIZE) {
			result = ISC_R_NOSPACE;
			goto done;
		}
		client->tcpbuf = isc_mem_get(client->mctx, TCP_BUFFER_SIZE);
		if (client->tcpbuf == NULL) {
			result = ISC_R_NOMEMORY;
			goto done;
		}
		data = client->tcpbuf;
		if (tcpbuffer != NULL) {
			isc_buffer_init(tcpbuffer, data, TCP_BUFFER_SIZE);
			isc_buffer_init(buffer, data + 2, TCP_BUFFER_SIZE - 2);
		} else {
			isc_buffer_init(buffer, data, TCP_BUFFER_SIZE);
			INSIST(length <= 0xffff);
			isc_buffer_putuint16(buffer, (uint16_t)length);
		}
	} else {
		data = sendbuf;
		if ((client->attributes & NS_CLIENTATTR_HAVECOOKIE) == 0) {
			if (client->view != NULL)
				bufsize = client->view->nocookieudp;
			else
				bufsize = 512;
		} else
			bufsize = client->udpsize;
		if (bufsize > client->udpsize)
			bufsize = client->udpsize;
		if (bufsize > SEND_BUFFER_SIZE)
			bufsize = SEND_BUFFER_SIZE;
		if (length > bufsize) {
			result = ISC_R_NOSPACE;
			goto done;
		}
		isc_buffer_init(buffer, data, bufsize);
	}
	*datap = data;
	result = ISC_R_SUCCESS;

 done:
	return (result);
}