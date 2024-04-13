LDAPRDN_validate( LDAPRDN rdn )
{
	int		iAVA;
	int 		rc;

	assert( rdn != NULL );

	for ( iAVA = 0; rdn[ iAVA ]; iAVA++ ) {
		LDAPAVA			*ava = rdn[ iAVA ];
		AttributeDescription	*ad;
		slap_syntax_validate_func *validate = NULL;

		assert( ava != NULL );
		
		if ( ( ad = AVA_PRIVATE( ava ) ) == NULL ) {
			const char	*text = NULL;

			rc = slap_bv2ad( &ava->la_attr, &ad, &text );
			if ( rc != LDAP_SUCCESS ) {
				rc = slap_bv2undef_ad( &ava->la_attr,
					&ad, &text,
					SLAP_AD_PROXIED|slap_DN_strict );
				if ( rc != LDAP_SUCCESS ) {
					return LDAP_INVALID_SYNTAX;
				}
			}

			ava->la_private = ( void * )ad;
		}

		/*
		 * Do not allow X-ORDERED 'VALUES' naming attributes
		 */
		if ( ad->ad_type->sat_flags & SLAP_AT_ORDERED_VAL ) {
			return LDAP_INVALID_SYNTAX;
		}

		/* 
		 * Replace attr oid/name with the canonical name
		 */
		ava->la_attr = ad->ad_cname;

		validate = ad->ad_type->sat_syntax->ssyn_validate;

		if ( validate ) {
			/*
		 	 * validate value by validate function
			 */
			rc = ( *validate )( ad->ad_type->sat_syntax,
				&ava->la_value );
			
			if ( rc != LDAP_SUCCESS ) {
				return LDAP_INVALID_SYNTAX;
			}
		}
	}

	return LDAP_SUCCESS;
}