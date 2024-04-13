void usbredirparser_send_interrupt_packet(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_interrupt_packet_header *interrupt_header,
    uint8_t *data, int data_len)
{
    usbredirparser_queue(parser, usb_redir_interrupt_packet, id,
                         interrupt_header, data, data_len);
}