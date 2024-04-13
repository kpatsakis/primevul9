dnPretty(
	Syntax *syntax,
	struct berval *val,
	struct berval *out,
	void *ctx)
{
	assert( val != NULL );
	assert( out != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> dnPretty: <%s>\n", val->bv_val ? val->bv_val : "", 0, 0 );

	if ( val->bv_len == 0 ) {
		ber_dupbv_x( out, val, ctx );

	} else if ( val->bv_len > SLAP_LDAPDN_MAXLEN ) {
		return LDAP_INVALID_SYNTAX;

	} else {
		LDAPDN		dn = NULL;
		int		rc;

		/* FIXME: should be liberal in what we accept */
		rc = ldap_bv2dn_x( val, &dn, LDAP_DN_FORMAT_LDAP, ctx );
		if ( rc != LDAP_SUCCESS ) {
			return LDAP_INVALID_SYNTAX;
		}

		assert( strlen( val->bv_val ) == val->bv_len );

		/*
		 * Schema-aware rewrite
		 */
		if ( LDAPDN_rewrite( dn, SLAP_LDAPDN_PRETTY, ctx ) != LDAP_SUCCESS ) {
			ldap_dnfree_x( dn, ctx );
			return LDAP_INVALID_SYNTAX;
		}

		/* FIXME: not sure why the default isn't pretty */
		/* RE: the default is the form that is used as
		 * an internal representation; the pretty form
		 * is a variant */
		rc = ldap_dn2bv_x( dn, out,
			LDAP_DN_FORMAT_LDAPV3 | LDAP_DN_PRETTY, ctx );

		ldap_dnfree_x( dn, ctx );

		if ( rc != LDAP_SUCCESS ) {
			return LDAP_INVALID_SYNTAX;
		}
	}

	Debug( LDAP_DEBUG_TRACE, "<<< dnPretty: <%s>\n", out->bv_val ? out->bv_val : "", 0, 0 );

	return LDAP_SUCCESS;
}