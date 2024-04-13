void usbredirparser_send_free_bulk_streams(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_free_bulk_streams_header *free_bulk_streams)
{
    usbredirparser_queue(parser, usb_redir_free_bulk_streams, id,
                         free_bulk_streams, NULL, 0);
}