LDAPDN_validate( LDAPDN dn )
{
	int 		iRDN;
	int 		rc;

	assert( dn != NULL );

	for ( iRDN = 0; dn[ iRDN ]; iRDN++ ) {
		rc = LDAPRDN_validate( dn[ iRDN ] );
		if ( rc != LDAP_SUCCESS ) {
			return rc;
		}
	}

	return LDAP_SUCCESS;
}