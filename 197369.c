void usbredirparser_send_start_interrupt_receiving(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_start_interrupt_receiving_header *start_interrupt_receiving)
{
    usbredirparser_queue(parser, usb_redir_start_interrupt_receiving, id,
                         start_interrupt_receiving, NULL, 0);
}