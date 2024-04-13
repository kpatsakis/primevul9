bool sctp_verify_asconf(const struct sctp_association *asoc,
			struct sctp_chunk *chunk, bool addr_param_needed,
			struct sctp_paramhdr **errp)
{
	struct sctp_addip_chunk *addip;
	bool addr_param_seen = false;
	union sctp_params param;

	addip = (struct sctp_addip_chunk *)chunk->chunk_hdr;
	sctp_walk_params(param, addip, addip_hdr.params) {
		size_t length = ntohs(param.p->length);

		*errp = param.p;
		switch (param.p->type) {
		case SCTP_PARAM_ERR_CAUSE:
			break;
		case SCTP_PARAM_IPV4_ADDRESS:
			if (length != sizeof(struct sctp_ipv4addr_param))
				return false;
			/* ensure there is only one addr param and it's in the
			 * beginning of addip_hdr params, or we reject it.
			 */
			if (param.v != addip->addip_hdr.params)
				return false;
			addr_param_seen = true;
			break;
		case SCTP_PARAM_IPV6_ADDRESS:
			if (length != sizeof(struct sctp_ipv6addr_param))
				return false;
			if (param.v != addip->addip_hdr.params)
				return false;
			addr_param_seen = true;
			break;
		case SCTP_PARAM_ADD_IP:
		case SCTP_PARAM_DEL_IP:
		case SCTP_PARAM_SET_PRIMARY:
			/* In ASCONF chunks, these need to be first. */
			if (addr_param_needed && !addr_param_seen)
				return false;
			length = ntohs(param.addip->param_hdr.length);
			if (length < sizeof(struct sctp_addip_param) +
				     sizeof(**errp))
				return false;
			break;
		case SCTP_PARAM_SUCCESS_REPORT:
		case SCTP_PARAM_ADAPTATION_LAYER_IND:
			if (length != sizeof(struct sctp_addip_param))
				return false;
			break;
		default:
			/* This is unkown to us, reject! */
			return false;
		}
	}

	/* Remaining sanity checks. */
	if (addr_param_needed && !addr_param_seen)
		return false;
	if (!addr_param_needed && addr_param_seen)
		return false;
	if (param.v != chunk->chunk_end)
		return false;

	return true;
}