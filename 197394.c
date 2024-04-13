void usbredirparser_send_set_configuration(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_set_configuration_header *set_configuration)
{
    usbredirparser_queue(parser, usb_redir_set_configuration, id,
                         set_configuration, NULL, 0);
}