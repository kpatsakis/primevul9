void usbredirparser_send_device_connect(struct usbredirparser *parser,
    struct usb_redir_device_connect_header *device_connect)
{
    usbredirparser_queue(parser, usb_redir_device_connect, 0, device_connect,
                         NULL, 0);
}