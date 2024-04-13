void usbredirparser_send_interface_info(struct usbredirparser *parser,
    struct usb_redir_interface_info_header *interface_info)
{
    usbredirparser_queue(parser, usb_redir_interface_info, 0, interface_info,
                         NULL, 0);
}