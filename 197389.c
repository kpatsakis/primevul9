void usbredirparser_send_configuration_status(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_configuration_status_header *configuration_status)
{
    usbredirparser_queue(parser, usb_redir_configuration_status, id,
                         configuration_status, NULL, 0);
}