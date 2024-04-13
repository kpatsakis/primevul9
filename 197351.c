static int usbredirparser_verify_bulk_recv_cap(
    struct usbredirparser *parser_pub, int send)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;

    if ((send && !usbredirparser_peer_has_cap(parser_pub,
                                              usb_redir_cap_bulk_receiving)) ||
        (!send && !usbredirparser_have_cap(parser_pub,
                                           usb_redir_cap_bulk_receiving))) {
        ERROR("error bulk_receiving without cap_bulk_receiving");
        return 0;
    }
    return 1; /* Verify ok */
}