void usbredirparser_init(struct usbredirparser *parser_pub,
    const char *version, uint32_t *caps, int caps_len, int flags)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    struct usb_redir_hello_header hello = { { 0 }, };

    parser->flags = (flags & ~usbredirparser_fl_no_hello);
    if (parser->callb.alloc_lock_func) {
        parser->lock = parser->callb.alloc_lock_func();
    }

    snprintf(hello.version, sizeof(hello.version), "%s", version);
    if (caps_len > USB_REDIR_CAPS_SIZE) {
        caps_len = USB_REDIR_CAPS_SIZE;
    }
    memcpy(parser->our_caps, caps, caps_len * sizeof(uint32_t));
    /* libusbredirparser handles sending the ack internally */
    if (!(flags & usbredirparser_fl_usb_host))
        usbredirparser_caps_set_cap(parser->our_caps,
                                    usb_redir_cap_device_disconnect_ack);
    usbredirparser_verify_caps(parser, parser->our_caps, "our");
    if (!(flags & usbredirparser_fl_no_hello))
        usbredirparser_queue(parser_pub, usb_redir_hello, 0, &hello,
                             (uint8_t *)parser->our_caps,
                             USB_REDIR_CAPS_SIZE * sizeof(uint32_t));
}