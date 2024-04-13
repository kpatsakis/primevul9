struct sctp_chunk *sctp_make_init_ack(const struct sctp_association *asoc,
				      const struct sctp_chunk *chunk,
				      gfp_t gfp, int unkparam_len)
{
	struct sctp_supported_ext_param ext_param;
	struct sctp_adaptation_ind_param aiparam;
	struct sctp_paramhdr *auth_chunks = NULL;
	struct sctp_paramhdr *auth_random = NULL;
	struct sctp_paramhdr *auth_hmacs = NULL;
	struct sctp_chunk *retval = NULL;
	struct sctp_cookie_param *cookie;
	struct sctp_inithdr initack;
	union sctp_params addrs;
	struct sctp_sock *sp;
	__u8 extensions[5];
	size_t chunksize;
	int num_ext = 0;
	int cookie_len;
	int addrs_len;

	/* Note: there may be no addresses to embed. */
	addrs = sctp_bind_addrs_to_raw(&asoc->base.bind_addr, &addrs_len, gfp);

	initack.init_tag	        = htonl(asoc->c.my_vtag);
	initack.a_rwnd			= htonl(asoc->rwnd);
	initack.num_outbound_streams	= htons(asoc->c.sinit_num_ostreams);
	initack.num_inbound_streams	= htons(asoc->c.sinit_max_instreams);
	initack.initial_tsn		= htonl(asoc->c.initial_tsn);

	/* FIXME:  We really ought to build the cookie right
	 * into the packet instead of allocating more fresh memory.
	 */
	cookie = sctp_pack_cookie(asoc->ep, asoc, chunk, &cookie_len,
				  addrs.v, addrs_len);
	if (!cookie)
		goto nomem_cookie;

	/* Calculate the total size of allocation, include the reserved
	 * space for reporting unknown parameters if it is specified.
	 */
	sp = sctp_sk(asoc->base.sk);
	chunksize = sizeof(initack) + addrs_len + cookie_len + unkparam_len;

	/* Tell peer that we'll do ECN only if peer advertised such cap.  */
	if (asoc->peer.ecn_capable)
		chunksize += sizeof(ecap_param);

	if (asoc->peer.prsctp_capable)
		chunksize += sizeof(prsctp_param);

	if (asoc->peer.asconf_capable) {
		extensions[num_ext] = SCTP_CID_ASCONF;
		extensions[num_ext+1] = SCTP_CID_ASCONF_ACK;
		num_ext += 2;
	}

	if (asoc->peer.reconf_capable) {
		extensions[num_ext] = SCTP_CID_RECONF;
		num_ext += 1;
	}

	if (sp->adaptation_ind)
		chunksize += sizeof(aiparam);

	if (asoc->intl_enable) {
		extensions[num_ext] = SCTP_CID_I_DATA;
		num_ext += 1;
	}

	if (asoc->peer.auth_capable) {
		auth_random = (struct sctp_paramhdr *)asoc->c.auth_random;
		chunksize += ntohs(auth_random->length);

		auth_hmacs = (struct sctp_paramhdr *)asoc->c.auth_hmacs;
		if (auth_hmacs->length)
			chunksize += SCTP_PAD4(ntohs(auth_hmacs->length));
		else
			auth_hmacs = NULL;

		auth_chunks = (struct sctp_paramhdr *)asoc->c.auth_chunks;
		if (auth_chunks->length)
			chunksize += SCTP_PAD4(ntohs(auth_chunks->length));
		else
			auth_chunks = NULL;

		extensions[num_ext] = SCTP_CID_AUTH;
		num_ext += 1;
	}

	if (num_ext)
		chunksize += SCTP_PAD4(sizeof(ext_param) + num_ext);

	/* Now allocate and fill out the chunk.  */
	retval = sctp_make_control(asoc, SCTP_CID_INIT_ACK, 0, chunksize, gfp);
	if (!retval)
		goto nomem_chunk;

	/* RFC 2960 6.4 Multi-homed SCTP Endpoints
	 *
	 * An endpoint SHOULD transmit reply chunks (e.g., SACK,
	 * HEARTBEAT ACK, * etc.) to the same destination transport
	 * address from which it received the DATA or control chunk
	 * to which it is replying.
	 *
	 * [INIT ACK back to where the INIT came from.]
	 */
	retval->transport = chunk->transport;

	retval->subh.init_hdr =
		sctp_addto_chunk(retval, sizeof(initack), &initack);
	retval->param_hdr.v = sctp_addto_chunk(retval, addrs_len, addrs.v);
	sctp_addto_chunk(retval, cookie_len, cookie);
	if (asoc->peer.ecn_capable)
		sctp_addto_chunk(retval, sizeof(ecap_param), &ecap_param);
	if (num_ext) {
		ext_param.param_hdr.type = SCTP_PARAM_SUPPORTED_EXT;
		ext_param.param_hdr.length = htons(sizeof(ext_param) + num_ext);
		sctp_addto_chunk(retval, sizeof(ext_param), &ext_param);
		sctp_addto_param(retval, num_ext, extensions);
	}
	if (asoc->peer.prsctp_capable)
		sctp_addto_chunk(retval, sizeof(prsctp_param), &prsctp_param);

	if (sp->adaptation_ind) {
		aiparam.param_hdr.type = SCTP_PARAM_ADAPTATION_LAYER_IND;
		aiparam.param_hdr.length = htons(sizeof(aiparam));
		aiparam.adaptation_ind = htonl(sp->adaptation_ind);
		sctp_addto_chunk(retval, sizeof(aiparam), &aiparam);
	}

	if (asoc->peer.auth_capable) {
		sctp_addto_chunk(retval, ntohs(auth_random->length),
				 auth_random);
		if (auth_hmacs)
			sctp_addto_chunk(retval, ntohs(auth_hmacs->length),
					auth_hmacs);
		if (auth_chunks)
			sctp_addto_chunk(retval, ntohs(auth_chunks->length),
					auth_chunks);
	}

	/* We need to remove the const qualifier at this point.  */
	retval->asoc = (struct sctp_association *) asoc;

nomem_chunk:
	kfree(cookie);
nomem_cookie:
	kfree(addrs.v);
	return retval;
}