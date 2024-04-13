dnX509peerNormalize( void *ssl, struct berval *dn )
{
	int rc = LDAP_INVALID_CREDENTIALS;

	if ( DNX509PeerNormalizeCertMap != NULL )
		rc = (*DNX509PeerNormalizeCertMap)( ssl, dn );

	if ( rc != LDAP_SUCCESS ) {
		rc = ldap_pvt_tls_get_peer_dn( ssl, dn,
			(LDAPDN_rewrite_dummy *)LDAPDN_rewrite, 0 );
	}

	return rc;
}