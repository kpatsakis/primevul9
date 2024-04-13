void usbredirparser_send_stop_iso_stream(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_stop_iso_stream_header *stop_iso_stream)
{
    usbredirparser_queue(parser, usb_redir_stop_iso_stream, id,
                         stop_iso_stream, NULL, 0);
}