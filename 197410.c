void usbredirparser_send_filter_reject(struct usbredirparser *parser)
{
    if (!usbredirparser_peer_has_cap(parser, usb_redir_cap_filter))
        return;

    usbredirparser_queue(parser, usb_redir_filter_reject, 0, NULL, NULL, 0);
}