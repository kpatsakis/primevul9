void usbredirparser_send_get_alt_setting(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_get_alt_setting_header *get_alt_setting)
{
    usbredirparser_queue(parser, usb_redir_get_alt_setting, id,
                         get_alt_setting, NULL, 0);
}