void usbredirparser_send_iso_packet(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_iso_packet_header *iso_header,
    uint8_t *data, int data_len)
{
    usbredirparser_queue(parser, usb_redir_iso_packet, id, iso_header,
                         data, data_len);
}