void usbredirparser_send_start_iso_stream(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_start_iso_stream_header *start_iso_stream)
{
    usbredirparser_queue(parser, usb_redir_start_iso_stream, id,
                         start_iso_stream, NULL, 0);
}