rdn_validate( struct berval *rdn )
{
#if 1
	/* Major cheat!
	 * input is a pretty or normalized DN
	 * hence, we can just search for ','
	 */
	if( rdn == NULL || rdn->bv_len == 0 ||
		rdn->bv_len > SLAP_LDAPDN_MAXLEN )
	{
		return LDAP_INVALID_SYNTAX;
	}
	return ber_bvchr( rdn, ',' ) == NULL
		? LDAP_SUCCESS : LDAP_INVALID_SYNTAX;

#else
	LDAPRDN		*RDN, **DN[ 2 ] = { &RDN, NULL };
	const char	*p;
	int		rc;

	/*
	 * must be non-empty
	 */
	if ( rdn == NULL || rdn == '\0' ) {
		return 0;
	}

	/*
	 * must be parsable
	 */
	rc = ldap_bv2rdn( rdn, &RDN, (char **)&p, LDAP_DN_FORMAT_LDAP );
	if ( rc != LDAP_SUCCESS ) {
		return 0;
	}

	/*
	 * Must be one-level
	 */
	if ( p[ 0 ] != '\0' ) {
		return 0;
	}

	/*
	 * Schema-aware validate
	 */
	if ( rc == LDAP_SUCCESS ) {
		rc = LDAPDN_validate( DN );
	}
	ldap_rdnfree( RDN );

	/*
	 * Must validate (there's a repeated parsing ...)
	 */
	return ( rc == LDAP_SUCCESS );
#endif
}