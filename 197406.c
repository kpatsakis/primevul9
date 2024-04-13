static int usbredirparser_expect_extra_data(struct usbredirparser_priv *parser)
{
    switch (parser->header.type) {
    case usb_redir_hello: /* For the variable length capabilities array */
    case usb_redir_filter_filter:
    case usb_redir_control_packet:
    case usb_redir_bulk_packet:
    case usb_redir_iso_packet:
    case usb_redir_interrupt_packet:
    case usb_redir_buffered_bulk_packet:
        return 1;
    default:
        return 0;
    }
}