void usbredirparser_send_bulk_receiving_status(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_bulk_receiving_status_header *bulk_receiving_status)
{
    usbredirparser_queue(parser, usb_redir_bulk_receiving_status, id,
                         bulk_receiving_status, NULL, 0);
}