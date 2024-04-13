build_new_dn( struct berval * new_dn,
	struct berval * parent_dn,
	struct berval * newrdn,
	void *memctx )
{
	char *ptr;

	if ( parent_dn == NULL || parent_dn->bv_len == 0 ) {
		ber_dupbv_x( new_dn, newrdn, memctx );
		return;
	}

	new_dn->bv_len = parent_dn->bv_len + newrdn->bv_len + 1;
	new_dn->bv_val = (char *) slap_sl_malloc( new_dn->bv_len + 1, memctx );

	ptr = lutil_strncopy( new_dn->bv_val, newrdn->bv_val, newrdn->bv_len );
	*ptr++ = ',';
	strcpy( ptr, parent_dn->bv_val );
}