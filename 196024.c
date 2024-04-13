LDAPRDN_rewrite( LDAPRDN rdn, unsigned flags, void *ctx )
{

	int rc, iAVA, do_sort = 0;

	for ( iAVA = 0; rdn[ iAVA ]; iAVA++ ) {
		LDAPAVA			*ava = rdn[ iAVA ];
		AttributeDescription	*ad;
		slap_syntax_validate_func *validf = NULL;
		slap_mr_normalize_func *normf = NULL;
		slap_syntax_transform_func *transf = NULL;
		MatchingRule *mr = NULL;
		struct berval		bv = BER_BVNULL;

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
			do_sort = 1;
		}

		/* 
		 * Replace attr oid/name with the canonical name
		 */
		ava->la_attr = ad->ad_cname;

		if( ava->la_flags & LDAP_AVA_BINARY ) {
			/* AVA is binary encoded, not supported */
			return LDAP_INVALID_SYNTAX;

			/* Do not allow X-ORDERED 'VALUES' naming attributes */
		} else if( ad->ad_type->sat_flags & SLAP_AT_ORDERED_VAL ) {
			return LDAP_INVALID_SYNTAX;

		} else if( flags & SLAP_LDAPDN_PRETTY ) {
			transf = ad->ad_type->sat_syntax->ssyn_pretty;
			if( !transf ) {
				validf = ad->ad_type->sat_syntax->ssyn_validate;
			}
		} else { /* normalization */
			validf = ad->ad_type->sat_syntax->ssyn_validate;
			mr = ad->ad_type->sat_equality;
			if( mr && (!( mr->smr_usage & SLAP_MR_MUTATION_NORMALIZER ))) {
				normf = mr->smr_normalize;
			}
		}

		if ( validf ) {
			/* validate value before normalization */
			rc = ( *validf )( ad->ad_type->sat_syntax,
				ava->la_value.bv_len
					? &ava->la_value
					: (struct berval *) &slap_empty_bv );

			if ( rc != LDAP_SUCCESS ) {
				return LDAP_INVALID_SYNTAX;
			}
		}

		if ( transf ) {
			/*
		 	 * transform value by pretty function
			 *	if value is empty, use empty_bv
			 */
			rc = ( *transf )( ad->ad_type->sat_syntax,
				ava->la_value.bv_len
					? &ava->la_value
					: (struct berval *) &slap_empty_bv,
				&bv, ctx );
		
			if ( rc != LDAP_SUCCESS ) {
				return LDAP_INVALID_SYNTAX;
			}
		}

		if ( normf ) {
			/*
		 	 * normalize value
			 *	if value is empty, use empty_bv
			 */
			rc = ( *normf )(
				SLAP_MR_VALUE_OF_ASSERTION_SYNTAX,
				ad->ad_type->sat_syntax,
				mr,
				ava->la_value.bv_len
					? &ava->la_value
					: (struct berval *) &slap_empty_bv,
				&bv, ctx );
		
			if ( rc != LDAP_SUCCESS ) {
				return LDAP_INVALID_SYNTAX;
			}
		}


		if( bv.bv_val ) {
			if ( ava->la_flags & LDAP_AVA_FREE_VALUE )
				ber_memfree_x( ava->la_value.bv_val, ctx );
			ava->la_value = bv;
			ava->la_flags |= LDAP_AVA_FREE_VALUE;
		}
		/* reject empty values */
		if (!ava->la_value.bv_len) {
			return LDAP_INVALID_SYNTAX;
		}
	}
	rc = LDAP_SUCCESS;

	if ( do_sort ) {
		rc = AVA_Sort( rdn, iAVA );
	}

	return rc;
}