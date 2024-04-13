void usbredirparser_send_bulk_streams_status(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_bulk_streams_status_header *bulk_streams_status)
{
    usbredirparser_queue(parser, usb_redir_bulk_streams_status, id,
                         bulk_streams_status, NULL, 0);
}