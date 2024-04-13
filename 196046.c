dnPrettyNormal(
	Syntax *syntax,
	struct berval *val,
	struct berval *pretty,
	struct berval *normal,
	void *ctx)
{
	assert( val != NULL );
	assert( pretty != NULL );
	assert( normal != NULL );
	Debug( LDAP_DEBUG_TRACE, ">>> dnPrettyNormal: <%s>\n", val->bv_val ? val->bv_val : "", 0, 0 );

	if ( val->bv_len == 0 ) {
		ber_dupbv_x( pretty, val, ctx );
		ber_dupbv_x( normal, val, ctx );

	} else if ( val->bv_len > SLAP_LDAPDN_MAXLEN ) {
		/* too big */
		return LDAP_INVALID_SYNTAX;

	} else {
		LDAPDN		dn = NULL;
		int		rc;

		pretty->bv_val = NULL;
		normal->bv_val = NULL;
		pretty->bv_len = 0;
		normal->bv_len = 0;

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

		rc = ldap_dn2bv_x( dn, pretty,
			LDAP_DN_FORMAT_LDAPV3 | LDAP_DN_PRETTY, ctx );

		if ( rc != LDAP_SUCCESS ) {
			ldap_dnfree_x( dn, ctx );
			return LDAP_INVALID_SYNTAX;
		}

		if ( LDAPDN_rewrite( dn, 0, ctx ) != LDAP_SUCCESS ) {
			ldap_dnfree_x( dn, ctx );
			ber_memfree_x( pretty->bv_val, ctx );
			pretty->bv_val = NULL;
			pretty->bv_len = 0;
			return LDAP_INVALID_SYNTAX;
		}

		rc = ldap_dn2bv_x( dn, normal,
			LDAP_DN_FORMAT_LDAPV3 | LDAP_DN_PRETTY, ctx );

		ldap_dnfree_x( dn, ctx );
		if ( rc != LDAP_SUCCESS ) {
			ber_memfree_x( pretty->bv_val, ctx );
			pretty->bv_val = NULL;
			pretty->bv_len = 0;
			return LDAP_INVALID_SYNTAX;
		}
	}

	Debug( LDAP_DEBUG_TRACE, "<<< dnPrettyNormal: <%s>, <%s>\n",
		pretty->bv_val ? pretty->bv_val : "",
		normal->bv_val ? normal->bv_val : "", 0 );

	return LDAP_SUCCESS;
}