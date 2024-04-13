static int usbredirparser_get_header_len(struct usbredirparser *parser_pub)
{
    if (usbredirparser_using_32bits_ids(parser_pub))
        return sizeof(struct usb_redir_header_32bit_id);
    else
        return sizeof(struct usb_redir_header);
}