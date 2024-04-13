static int usbredirparser_using_32bits_ids(struct usbredirparser *parser_pub)
{
    return !usbredirparser_have_cap(parser_pub, usb_redir_cap_64bits_ids) ||
           !usbredirparser_peer_has_cap(parser_pub, usb_redir_cap_64bits_ids);
}