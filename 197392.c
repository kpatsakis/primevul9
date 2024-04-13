void usbredirparser_send_buffered_bulk_packet(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_buffered_bulk_packet_header *buffered_bulk_header,
    uint8_t *data, int data_len)
{
    usbredirparser_queue(parser, usb_redir_buffered_bulk_packet, id,
                         buffered_bulk_header, data, data_len);
}