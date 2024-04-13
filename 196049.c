rdnValidate(
	Syntax *syntax,
	struct berval *in )
{
	int		rc;
	LDAPRDN		rdn;
	char*		p;

	assert( in != NULL );
	if ( in->bv_len == 0 ) {
		return LDAP_SUCCESS;

	} else if ( in->bv_len > SLAP_LDAPDN_MAXLEN ) {
		return LDAP_INVALID_SYNTAX;
	}

	rc = ldap_bv2rdn_x( in , &rdn, (char **) &p,
				LDAP_DN_FORMAT_LDAP, NULL);
	if ( rc != LDAP_SUCCESS ) {
		return LDAP_INVALID_SYNTAX;
	}

	assert( strlen( in->bv_val ) == in->bv_len );

	/*
	 * Schema-aware validate
	 */
	rc = LDAPRDN_validate( rdn );
	ldap_rdnfree( rdn );

	if ( rc != LDAP_SUCCESS ) {
		return LDAP_INVALID_SYNTAX;
	}

	return LDAP_SUCCESS;
}