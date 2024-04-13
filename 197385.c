void usbredirparser_send_cancel_data_packet(struct usbredirparser *parser,
    uint64_t id)
{
    usbredirparser_queue(parser, usb_redir_cancel_data_packet, id,
                         NULL, NULL, 0);
}