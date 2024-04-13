int register_certificate_map_function(SLAP_CERT_MAP_FN *fn)
{
#ifdef HAVE_TLS
	if ( DNX509PeerNormalizeCertMap == NULL ) {
		DNX509PeerNormalizeCertMap = fn;
		return 0;
	}
#endif

	return -1;
}