dnRdn( 
	struct berval	*dn, 
	struct berval	*rdn )
{
	char	*p;

	*rdn = *dn;
	p = ber_bvchr( dn, ',' );

	/* one-level dn */
	if ( p == NULL ) {
		return;
	}

	assert( DN_SEPARATOR( p[ 0 ] ) );
	assert( ATTR_LEADCHAR( p[ 1 ] ) );
	rdn->bv_len = p - dn->bv_val;

	return;
}