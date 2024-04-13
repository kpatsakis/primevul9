GlobusJobStatusName( int status )
{
	static char buf[GRAM_STATUS_STR_LEN];
#if defined(HAVE_EXT_GLOBUS)
	switch ( status ) {
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_PENDING:
		return "PENDING";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_ACTIVE:
		return "ACTIVE";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED:
		return "FAILED";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE:
		return "DONE";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_SUSPENDED:
		return "SUSPENDED";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED:
		return "UNSUBMITTED";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_IN:
		return "STAGE_IN";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_OUT:
		return "STAGE_OUT";
	case GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNKNOWN:
		return "UNKNOWN";
	default:
		snprintf( buf, GRAM_STATUS_STR_LEN, "%d", status );
		return buf;
	}
#else
	snprintf( buf, GRAM_STATUS_STR_LEN, "%d", status );
	return buf;
#endif
}