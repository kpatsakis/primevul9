dn_rdnlen(
	Backend		*be,
	struct berval	*dn_in )
{
	const char	*p;

	assert( dn_in != NULL );

	if ( dn_in == NULL ) {
		return 0;
	}

	if ( !dn_in->bv_len ) {
		return 0;
	}

	if ( be != NULL && be_issuffix( be, dn_in ) ) {
		return 0;
	}

	p = ber_bvchr( dn_in, ',' );

	return p ? (ber_len_t) (p - dn_in->bv_val) : dn_in->bv_len;
}