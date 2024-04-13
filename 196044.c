dnIsSuffix(
	const struct berval *dn,
	const struct berval *suffix )
{
	int	d;

	assert( dn != NULL );
	assert( suffix != NULL );

	d = dn->bv_len - suffix->bv_len;

	/* empty suffix matches any dn */
	if ( suffix->bv_len == 0 ) {
		return 1;
	}

	/* suffix longer than dn */
	if ( d < 0 ) {
		return 0;
	}

	/* no rdn separator or escaped rdn separator */
	if ( d > 1 && !DN_SEPARATOR( dn->bv_val[ d - 1 ] ) ) {
		return 0;
	}

	/* no possible match or malformed dn */
	if ( d == 1 ) {
		return 0;
	}

	/* compare */
	return( strncmp( dn->bv_val + d, suffix->bv_val, suffix->bv_len ) == 0 );
}