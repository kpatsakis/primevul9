static void usbredirparser_handle_hello(struct usbredirparser *parser_pub,
    struct usb_redir_hello_header *hello, uint8_t *data, int data_len)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    uint32_t *peer_caps = (uint32_t *)data;
    char buf[64];
    int i;

    if (parser->have_peer_caps) {
        ERROR("Received second hello message, ignoring");
        return;
    }

    /* In case hello->version is not 0 terminated (which would be a protocol
       violation)_ */
    strncpy(buf, hello->version, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    memset(parser->peer_caps, 0, sizeof(parser->peer_caps));
    if (data_len > sizeof(parser->peer_caps)) {
        data_len = sizeof(parser->peer_caps);
    }
    for (i = 0; i < data_len / sizeof(uint32_t); i++) {
        parser->peer_caps[i] = peer_caps[i];
    }
    usbredirparser_verify_caps(parser, parser->peer_caps, "peer");
    parser->have_peer_caps = 1;

    INFO("Peer version: %s, using %d-bits ids", buf,
         usbredirparser_using_32bits_ids(parser_pub) ? 32 : 64);

    /* Added in 0.3.2, so no guarantee it is there */
    if (parser->callb.hello_func)
        parser->callb.hello_func(parser->callb.priv, hello);
}