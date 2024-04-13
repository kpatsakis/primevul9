void usbredirparser_send_set_alt_setting(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_set_alt_setting_header *set_alt_setting)
{
    usbredirparser_queue(parser, usb_redir_set_alt_setting, id,
                         set_alt_setting, NULL, 0);
}