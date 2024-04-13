void usbredirparser_send_alloc_bulk_streams(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_alloc_bulk_streams_header *alloc_bulk_streams)
{
    usbredirparser_queue(parser, usb_redir_alloc_bulk_streams, id,
                         alloc_bulk_streams, NULL, 0);
}