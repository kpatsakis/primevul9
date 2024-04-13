void usbredirparser_send_ep_info(struct usbredirparser *parser,
    struct usb_redir_ep_info_header *ep_info)
{
    usbredirparser_queue(parser, usb_redir_ep_info, 0, ep_info, NULL, 0);
}