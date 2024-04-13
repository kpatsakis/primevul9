void usbredirparser_send_stop_interrupt_receiving(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_stop_interrupt_receiving_header *stop_interrupt_receiving)
{
    usbredirparser_queue(parser, usb_redir_stop_interrupt_receiving, id,
                         stop_interrupt_receiving, NULL, 0);
}