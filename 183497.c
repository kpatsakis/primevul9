inline static int is_read_comp_null(gnutls_session_t session)
{
    if (session->security_parameters.read_compression_algorithm ==
	GNUTLS_COMP_NULL)
	return 0;

    return 1;
}