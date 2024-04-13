static int ssl_parse_session_ticket_ext( ssl_context *ssl,
                                         unsigned char *buf,
                                         size_t len )
{
    int ret;

    if( ssl->session_tickets == SSL_SESSION_TICKETS_DISABLED )
        return( 0 );

    /* Remember the client asked us to send a new ticket */
    ssl->handshake->new_session_ticket = 1;

    SSL_DEBUG_MSG( 3, ( "ticket length: %d", len ) );

    if( len == 0 )
        return( 0 );

    if( ssl->renegotiation != SSL_INITIAL_HANDSHAKE )
    {
        SSL_DEBUG_MSG( 3, ( "ticket rejected: renegotiating" ) );
        return( 0 );
    }

    /*
     * Failures are ok: just ignore the ticket and proceed.
     */
    if( ( ret = ssl_parse_ticket( ssl, buf, len ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_parse_ticket", ret );
        return( 0 );
    }

    SSL_DEBUG_MSG( 3, ( "session successfully restored from ticket" ) );

    ssl->handshake->resume = 1;

    /* Don't send a new ticket after all, this one is OK */
    ssl->handshake->new_session_ticket = 0;

    return( 0 );
}