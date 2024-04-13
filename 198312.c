struct sctp_chunk *sctp_make_heartbeat(const struct sctp_association *asoc,
				       const struct sctp_transport *transport)
{
	struct sctp_sender_hb_info hbinfo;
	struct sctp_chunk *retval;

	retval = sctp_make_control(asoc, SCTP_CID_HEARTBEAT, 0,
				   sizeof(hbinfo), GFP_ATOMIC);

	if (!retval)
		goto nodata;

	hbinfo.param_hdr.type = SCTP_PARAM_HEARTBEAT_INFO;
	hbinfo.param_hdr.length = htons(sizeof(hbinfo));
	hbinfo.daddr = transport->ipaddr;
	hbinfo.sent_at = jiffies;
	hbinfo.hb_nonce = transport->hb_nonce;

	/* Cast away the 'const', as this is just telling the chunk
	 * what transport it belongs to.
	 */
	retval->transport = (struct sctp_transport *) transport;
	retval->subh.hbs_hdr = sctp_addto_chunk(retval, sizeof(hbinfo),
						&hbinfo);

nodata:
	return retval;
}