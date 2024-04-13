void usbredirparser_send_alt_setting_status(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_alt_setting_status_header *alt_setting_status)
{
    usbredirparser_queue(parser, usb_redir_alt_setting_status, id,
                         alt_setting_status, NULL, 0);
}