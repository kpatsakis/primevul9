rdnMatch(
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
	
	match = value->bv_len - asserted->bv_len;

	if ( match == 0 ) {
		match = memcmp( value->bv_val, asserted->bv_val, 
				value->bv_len );
	}

	Debug( LDAP_DEBUG_ARGS, "rdnMatch %d\n\t\"%s\"\n\t\"%s\"\n",
		match, value->bv_val, asserted->bv_val );

	*matchp = match;
	return LDAP_SUCCESS;
}