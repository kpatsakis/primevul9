struct sctp_chunk *sctp_make_violation_max_retrans(
					const struct sctp_association *asoc,
					const struct sctp_chunk *chunk)
{
	static const char error[] = "Association exceeded its max_retans count";
	size_t payload_len = sizeof(error) + sizeof(struct sctp_errhdr);
	struct sctp_chunk *retval;

	retval = sctp_make_abort(asoc, chunk, payload_len);
	if (!retval)
		goto nodata;

	sctp_init_cause(retval, SCTP_ERROR_PROTO_VIOLATION, sizeof(error));
	sctp_addto_chunk(retval, sizeof(error), error);

nodata:
	return retval;
}