void usbredirparser_send_stop_bulk_receiving(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_stop_bulk_receiving_header *stop_bulk_receiving)
{
    usbredirparser_queue(parser, usb_redir_stop_bulk_receiving, id,
                         stop_bulk_receiving, NULL, 0);
}