dnPrettyNormalDN(
	Syntax *syntax,
	struct berval *val,
	LDAPDN *dn,
	int flags,
	void *ctx )
{
	assert( val != NULL );
	assert( dn != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> dn%sDN: <%s>\n", 
			flags == SLAP_LDAPDN_PRETTY ? "Pretty" : "Normal", 
			val->bv_val ? val->bv_val : "", 0 );

	if ( val->bv_len == 0 ) {
		return LDAP_SUCCESS;

	} else if ( val->bv_len > SLAP_LDAPDN_MAXLEN ) {
		return LDAP_INVALID_SYNTAX;

	} else {
		int		rc;

		/* FIXME: should be liberal in what we accept */
		rc = ldap_bv2dn_x( val, dn, LDAP_DN_FORMAT_LDAP, ctx );
		if ( rc != LDAP_SUCCESS ) {
			return LDAP_INVALID_SYNTAX;
		}

		assert( strlen( val->bv_val ) == val->bv_len );

		/*
		 * Schema-aware rewrite
		 */
		if ( LDAPDN_rewrite( *dn, flags, ctx ) != LDAP_SUCCESS ) {
			ldap_dnfree_x( *dn, ctx );
			*dn = NULL;
			return LDAP_INVALID_SYNTAX;
		}
	}

	Debug( LDAP_DEBUG_TRACE, "<<< dn%sDN\n", 
			flags == SLAP_LDAPDN_PRETTY ? "Pretty" : "Normal",
			0, 0 );

	return LDAP_SUCCESS;
}