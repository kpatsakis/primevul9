void usbredirparser_send_control_packet(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_control_packet_header *control_header,
    uint8_t *data, int data_len)
{
    usbredirparser_queue(parser, usb_redir_control_packet, id, control_header,
                         data, data_len);
}