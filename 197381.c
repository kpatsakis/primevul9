void usbredirparser_send_device_disconnect(struct usbredirparser *parser)
{
    usbredirparser_queue(parser, usb_redir_device_disconnect, 0, NULL,
                         NULL, 0);
}