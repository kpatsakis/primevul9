dnExtractRdn( 
	struct berval	*dn, 
	struct berval 	*rdn,
	void *ctx )
{
	LDAPRDN		tmpRDN;
	const char	*p;
	int		rc;

	assert( dn != NULL );
	assert( rdn != NULL );

	if( dn->bv_len == 0 ) {
		return LDAP_OTHER;
	}

	rc = ldap_bv2rdn_x( dn, &tmpRDN, (char **)&p, LDAP_DN_FORMAT_LDAP, ctx );
	if ( rc != LDAP_SUCCESS ) {
		return rc;
	}

	rc = ldap_rdn2bv_x( tmpRDN, rdn, LDAP_DN_FORMAT_LDAPV3 | LDAP_DN_PRETTY,
		ctx );

	ldap_rdnfree_x( tmpRDN, ctx );
	return rc;
}