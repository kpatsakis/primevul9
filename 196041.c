dnIsSuffixScope( struct berval *ndn, struct berval *nbase, int scope )
{
	if ( !dnIsSuffix( ndn, nbase ) ) {
		return 0;
	}

	return dnIsWithinScope( ndn, nbase, scope );
}