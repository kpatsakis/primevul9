static inline struct sctp_chunk *sctp_make_op_error_fixed(
					const struct sctp_association *asoc,
					const struct sctp_chunk *chunk)
{
	size_t size = asoc ? asoc->pathmtu : 0;

	if (!size)
		size = SCTP_DEFAULT_MAXSEGMENT;

	return sctp_make_op_error_space(asoc, chunk, size);
}