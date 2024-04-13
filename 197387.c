void usbredirparser_send_start_bulk_receiving(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_start_bulk_receiving_header *start_bulk_receiving)
{
    usbredirparser_queue(parser, usb_redir_start_bulk_receiving, id,
                         start_bulk_receiving, NULL, 0);
}