dnRelativeMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	int match;
	struct berval *asserted = (struct berval *) assertedValue;

	assert( matchp != NULL );
	assert( value != NULL );
	assert( assertedValue != NULL );
	assert( !BER_BVISNULL( value ) );
	assert( !BER_BVISNULL( asserted ) );

	if( mr == slap_schema.si_mr_dnSubtreeMatch ) {
		if( asserted->bv_len > value->bv_len ) {
			match = -1;
		} else if ( asserted->bv_len == value->bv_len ) {
			match = memcmp( value->bv_val, asserted->bv_val, 
				value->bv_len );
		} else {
			if( DN_SEPARATOR(
				value->bv_val[value->bv_len - asserted->bv_len - 1] ))
			{
				match = memcmp(
					&value->bv_val[value->bv_len - asserted->bv_len],
					asserted->bv_val, 
					asserted->bv_len );
			} else {
				match = 1;
			}
		}

		*matchp = match;
		return LDAP_SUCCESS;
	}

	if( mr == slap_schema.si_mr_dnSuperiorMatch ) {
		asserted = value;
		value = (struct berval *) assertedValue;
		mr = slap_schema.si_mr_dnSubordinateMatch;
	}

	if( mr == slap_schema.si_mr_dnSubordinateMatch ) {
		if( asserted->bv_len >= value->bv_len ) {
			match = -1;
		} else {
			if( DN_SEPARATOR(
				value->bv_val[value->bv_len - asserted->bv_len - 1] ))
			{
				match = memcmp(
					&value->bv_val[value->bv_len - asserted->bv_len],
					asserted->bv_val, 
					asserted->bv_len );
			} else {
				match = 1;
			}
		}

		*matchp = match;
		return LDAP_SUCCESS;
	}

	if( mr == slap_schema.si_mr_dnOneLevelMatch ) {
		if( asserted->bv_len >= value->bv_len ) {
			match = -1;
		} else {
			if( DN_SEPARATOR(
				value->bv_val[value->bv_len - asserted->bv_len - 1] ))
			{
				match = memcmp(
					&value->bv_val[value->bv_len - asserted->bv_len],
					asserted->bv_val, 
					asserted->bv_len );

				if( !match ) {
					struct berval rdn;
					rdn.bv_val = value->bv_val;
					rdn.bv_len = value->bv_len - asserted->bv_len - 1;
					match = dnIsOneLevelRDN( &rdn ) ? 0 : 1;
				}
			} else {
				match = 1;
			}
		}

		*matchp = match;
		return LDAP_SUCCESS;
	}

	/* should not be reachable */
	assert( 0 );
	return LDAP_OTHER;
}