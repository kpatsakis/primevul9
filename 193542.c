static int ssl_key_matches_curves( pk_context *pk,
                                   const ecp_curve_info **curves )
{
    const ecp_curve_info **crv = curves;
    ecp_group_id grp_id = pk_ec( *pk )->grp.id;

    while( *crv != NULL )
    {
        if( (*crv)->grp_id == grp_id )
            return( 1 );
        crv++;
    }

    return( 0 );
}