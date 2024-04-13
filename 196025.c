rdnNormalize(
    slap_mask_t use,
    Syntax *syntax,
    MatchingRule *mr,
    struct berval *val,
    struct berval *out,
    void *ctx)
{
	assert( val != NULL );
	assert( out != NULL );

	Debug( LDAP_DEBUG_TRACE, ">>> dnNormalize: <%s>\n", val->bv_val ? val->bv_val : "", 0, 0 );
	if ( val->bv_len != 0 ) {
		LDAPRDN		rdn = NULL;
		int		rc;
		char*		p;

		/*
		 * Go to structural representation
		 */
		rc = ldap_bv2rdn_x( val , &rdn, (char **) &p,
					LDAP_DN_FORMAT_LDAP, ctx);

		if ( rc != LDAP_SUCCESS ) {
			return LDAP_INVALID_SYNTAX;
		}

		assert( strlen( val->bv_val ) == val->bv_len );

		/*
		 * Schema-aware rewrite
		 */
		if ( LDAPRDN_rewrite( rdn, 0, ctx ) != LDAP_SUCCESS ) {
			ldap_rdnfree_x( rdn, ctx );
			return LDAP_INVALID_SYNTAX;
		}

		/*
		 * Back to string representation
		 */
		rc = ldap_rdn2bv_x( rdn, out,
			LDAP_DN_FORMAT_LDAPV3 | LDAP_DN_PRETTY, ctx );

		ldap_rdnfree_x( rdn, ctx );

		if ( rc != LDAP_SUCCESS ) {
			return LDAP_INVALID_SYNTAX;
		}
	} else {
		ber_dupbv_x( out, val, ctx );
	}

	Debug( LDAP_DEBUG_TRACE, "<<< dnNormalize: <%s>\n", out->bv_val ? out->bv_val : "", 0, 0 );

	return LDAP_SUCCESS;
}