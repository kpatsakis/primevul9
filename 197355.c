void usbredirparser_send_get_configuration(struct usbredirparser *parser,
    uint64_t id)
{
    usbredirparser_queue(parser, usb_redir_get_configuration, id,
                         NULL, NULL, 0);
}