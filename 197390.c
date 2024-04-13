void usbredirparser_send_interrupt_receiving_status(struct usbredirparser *parser,
    uint64_t id,
    struct usb_redir_interrupt_receiving_status_header *interrupt_receiving_status)
{
    usbredirparser_queue(parser, usb_redir_interrupt_receiving_status, id,
                         interrupt_receiving_status, NULL, 0);
}