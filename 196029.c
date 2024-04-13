LDAPDN_rewrite( LDAPDN dn, unsigned flags, void *ctx )
{
	int 		iRDN;
	int 		rc;

	assert( dn != NULL );

	for ( iRDN = 0; dn[ iRDN ]; iRDN++ ) {
		rc = LDAPRDN_rewrite( dn[ iRDN ], flags, ctx );
		if ( rc != LDAP_SUCCESS ) {
			return rc;
		}
	}

	return LDAP_SUCCESS;
}