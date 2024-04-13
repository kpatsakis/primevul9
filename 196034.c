dnIsOneLevelRDN( struct berval *rdn )
{
	ber_len_t	len = rdn->bv_len;
	for ( ; len--; ) {
		if ( DN_SEPARATOR( rdn->bv_val[ len ] ) ) {
			return 0;
		}
	}

	return 1;
}