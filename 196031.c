dnIsWithinScope( struct berval *ndn, struct berval *nbase, int scope )
{
	assert( ndn != NULL );
	assert( nbase != NULL );
	assert( !BER_BVISNULL( ndn ) );
	assert( !BER_BVISNULL( nbase ) );

	switch ( scope ) {
	case LDAP_SCOPE_DEFAULT:
	case LDAP_SCOPE_SUBTREE:
		break;

	case LDAP_SCOPE_BASE:
		if ( ndn->bv_len != nbase->bv_len ) {
			return 0;
		}
		break;

	case LDAP_SCOPE_ONELEVEL: {
		struct berval pndn;
		dnParent( ndn, &pndn );
		if ( pndn.bv_len != nbase->bv_len ) {
			return 0;
		}
		} break;

	case LDAP_SCOPE_SUBORDINATE:
		if ( ndn->bv_len == nbase->bv_len ) {
			return 0;
		}
		break;

	/* unknown scope */
	default:
		return -1;
	}

	return 1;
}