void usbredirparser_send_reset(struct usbredirparser *parser)
{
    usbredirparser_queue(parser, usb_redir_reset, 0, NULL, NULL, 0);
}