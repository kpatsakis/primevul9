dnParent( 
	struct berval	*dn, 
	struct berval	*pdn )
{
	char	*p;

	p = ber_bvchr( dn, ',' );

	/* one-level dn */
	if ( p == NULL ) {
		pdn->bv_val = dn->bv_val + dn->bv_len;
		pdn->bv_len = 0;
		return;
	}

	assert( DN_SEPARATOR( p[ 0 ] ) );
	p++;

	assert( ATTR_LEADCHAR( p[ 0 ] ) );
	pdn->bv_len = dn->bv_len - (p - dn->bv_val);
	pdn->bv_val = p;

	return;
}