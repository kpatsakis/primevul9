void usbredirparser_send_bulk_packet(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_bulk_packet_header *bulk_header,
    uint8_t *data, int data_len)
{
    usbredirparser_queue(parser, usb_redir_bulk_packet, id, bulk_header,
                         data, data_len);
}