struct sctp_chunk *sctp_make_cookie_ack(const struct sctp_association *asoc,
					const struct sctp_chunk *chunk)
{
	struct sctp_chunk *retval;

	retval = sctp_make_control(asoc, SCTP_CID_COOKIE_ACK, 0, 0, GFP_ATOMIC);

	/* RFC 2960 6.4 Multi-homed SCTP Endpoints
	 *
	 * An endpoint SHOULD transmit reply chunks (e.g., SACK,
	 * HEARTBEAT ACK, * etc.) to the same destination transport
	 * address from which it * received the DATA or control chunk
	 * to which it is replying.
	 *
	 * [COOKIE ACK back to where the COOKIE ECHO came from.]
	 */
	if (retval && chunk)
		retval->transport = chunk->transport;

	return retval;
}