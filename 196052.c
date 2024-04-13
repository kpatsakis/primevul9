dnValidate(
	Syntax *syntax,
	struct berval *in )
{
	int		rc;
	LDAPDN		dn = NULL;

	assert( in != NULL );

	if ( in->bv_len == 0 ) {
		return LDAP_SUCCESS;

	} else if ( in->bv_len > SLAP_LDAPDN_MAXLEN ) {
		return LDAP_INVALID_SYNTAX;
	}

	rc = ldap_bv2dn( in, &dn, LDAP_DN_FORMAT_LDAP );
	if ( rc != LDAP_SUCCESS ) {
		return LDAP_INVALID_SYNTAX;
	}

	assert( strlen( in->bv_val ) == in->bv_len );

	/*
	 * Schema-aware validate
	 */
	rc = LDAPDN_validate( dn );
	ldap_dnfree( dn );

	if ( rc != LDAP_SUCCESS ) {
		return LDAP_INVALID_SYNTAX;
	}

	return LDAP_SUCCESS;
}