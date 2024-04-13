void usbredirparser_send_iso_stream_status(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_iso_stream_status_header *iso_stream_status)
{
    usbredirparser_queue(parser, usb_redir_iso_stream_status, id,
                         iso_stream_status, NULL, 0);
}