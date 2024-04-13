static int usbredirparser_verify_type_header(
    struct usbredirparser *parser_pub,
    int32_t type, void *header, uint8_t *data, int data_len, int send)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    int command_for_host = 0, expect_extra_data = 0;
    uint32_t length = 0;
    int ep = -1;

    if (parser->flags & usbredirparser_fl_usb_host) {
        command_for_host = 1;
    }
    if (send) {
        command_for_host = !command_for_host;
    }

    switch (type) {
    case usb_redir_interface_info: {
        struct usb_redir_interface_info_header *intf_info = header;

        if (intf_info->interface_count > 32) {
            ERROR("error interface_count > 32");
            return 0;
        }
        break;
    }
    case usb_redir_start_interrupt_receiving: {
        struct usb_redir_start_interrupt_receiving_header *start_int = header;

        if (!(start_int->endpoint & 0x80)) {
            ERROR("start int receiving on non input ep %02x",
                  start_int->endpoint);
            return 0;
        }
        break;
    }
    case usb_redir_stop_interrupt_receiving: {
        struct usb_redir_stop_interrupt_receiving_header *stop_int = header;

        if (!(stop_int->endpoint & 0x80)) {
            ERROR("stop int receiving on non input ep %02x",
                  stop_int->endpoint);
            return 0;
        }
        break;
    }
    case usb_redir_interrupt_receiving_status: {
        struct usb_redir_interrupt_receiving_status_header *int_status = header;

        if (!(int_status->endpoint & 0x80)) {
            ERROR("int receiving status for non input ep %02x",
                  int_status->endpoint);
            return 0;
        }
        break;
    }
    case usb_redir_filter_reject:
        if ((send && !usbredirparser_peer_has_cap(parser_pub,
                                             usb_redir_cap_filter)) ||
            (!send && !usbredirparser_have_cap(parser_pub,
                                             usb_redir_cap_filter))) {
            ERROR("error filter_reject without cap_filter");
            return 0;
        }
        break;
    case usb_redir_filter_filter:
        if ((send && !usbredirparser_peer_has_cap(parser_pub,
                                             usb_redir_cap_filter)) ||
            (!send && !usbredirparser_have_cap(parser_pub,
                                             usb_redir_cap_filter))) {
            ERROR("error filter_filter without cap_filter");
            return 0;
        }
        if (data_len < 1) {
            ERROR("error filter_filter without data");
            return 0;
        }
        if (data[data_len - 1] != 0) {
            ERROR("error non 0 terminated filter_filter data");
            return 0;
        }
        break;
    case usb_redir_device_disconnect_ack:
        if ((send && !usbredirparser_peer_has_cap(parser_pub,
                                     usb_redir_cap_device_disconnect_ack)) ||
            (!send && !usbredirparser_have_cap(parser_pub,
                                     usb_redir_cap_device_disconnect_ack))) {
            ERROR("error device_disconnect_ack without cap_device_disconnect_ack");
            return 0;
        }
        break;
    case usb_redir_start_bulk_receiving: {
        struct usb_redir_start_bulk_receiving_header *start_bulk = header;

        if (!usbredirparser_verify_bulk_recv_cap(parser_pub, send)) {
            return 0;
        }
        if (start_bulk->bytes_per_transfer > MAX_BULK_TRANSFER_SIZE) {
            ERROR("start bulk receiving length exceeds limits %u > %u",
                  start_bulk->bytes_per_transfer, MAX_BULK_TRANSFER_SIZE);
            return 0;
        }
        if (!(start_bulk->endpoint & 0x80)) {
            ERROR("start bulk receiving on non input ep %02x",
                  start_bulk->endpoint);
            return 0;
        }
        break;
    }
    case usb_redir_stop_bulk_receiving: {
        struct usb_redir_stop_bulk_receiving_header *stop_bulk = header;

        if (!usbredirparser_verify_bulk_recv_cap(parser_pub, send)) {
            return 0;
        }
        if (!(stop_bulk->endpoint & 0x80)) {
            ERROR("stop bulk receiving on non input ep %02x",
                  stop_bulk->endpoint);
            return 0;
        }
        break;
    }
    case usb_redir_bulk_receiving_status: {
        struct usb_redir_bulk_receiving_status_header *bulk_status = header;

        if (!usbredirparser_verify_bulk_recv_cap(parser_pub, send)) {
            return 0;
        }
        if (!(bulk_status->endpoint & 0x80)) {
            ERROR("bulk receiving status for non input ep %02x",
                  bulk_status->endpoint);
            return 0;
        }
        break;
    }
    case usb_redir_control_packet:
        length = ((struct usb_redir_control_packet_header *)header)->length;
        ep = ((struct usb_redir_control_packet_header *)header)->endpoint;
        break;
    case usb_redir_bulk_packet: {
        struct usb_redir_bulk_packet_header *bulk_packet = header;
        if (usbredirparser_have_cap(parser_pub,
                                usb_redir_cap_32bits_bulk_length) &&
            usbredirparser_peer_has_cap(parser_pub,
                                usb_redir_cap_32bits_bulk_length)) {
            length = (((uint32_t)bulk_packet->length_high) << 16) | bulk_packet->length;
        } else {
            length = bulk_packet->length;
            if (!send)
                bulk_packet->length_high = 0;
        }
        if (length > MAX_BULK_TRANSFER_SIZE) {
            ERROR("bulk transfer length exceeds limits %u > %u",
                  (uint32_t)length, MAX_BULK_TRANSFER_SIZE);
            return 0;
        }
        ep = bulk_packet->endpoint;
        break;
    }
    case usb_redir_iso_packet:
        length = ((struct usb_redir_iso_packet_header *)header)->length;
        ep = ((struct usb_redir_iso_packet_header *)header)->endpoint;
        break;
    case usb_redir_interrupt_packet:
        length = ((struct usb_redir_interrupt_packet_header *)header)->length;
        ep = ((struct usb_redir_interrupt_packet_header *)header)->endpoint;
        break;
    case usb_redir_buffered_bulk_packet: {
        struct usb_redir_buffered_bulk_packet_header *buf_bulk_pkt = header;
        length = buf_bulk_pkt->length;
        if (!usbredirparser_verify_bulk_recv_cap(parser_pub, send)) {
            return 0;
        }
        if ((uint32_t)length > MAX_BULK_TRANSFER_SIZE) {
            ERROR("buffered bulk transfer length exceeds limits %u > %u",
                  (uint32_t)length, MAX_BULK_TRANSFER_SIZE);
            return 0;
        }
        ep = buf_bulk_pkt->endpoint;
        break;
    }
    }

    if (ep != -1) {
        if (((ep & 0x80) && !command_for_host) ||
            (!(ep & 0x80) && command_for_host)) {
            expect_extra_data = 1;
        }
        if (expect_extra_data) {
            if (data_len != length) {
                ERROR("error data len %d != header len %d ep %02X",
                      data_len, length, ep);
                return 0;
            }
        } else {
            if (data || data_len) {
                ERROR("error unexpected extra data ep %02X", ep);
                return 0;
            }
            switch (type) {
            case usb_redir_iso_packet:
                ERROR("error iso packet send in wrong direction");
                return 0;
            case usb_redir_interrupt_packet:
                if (command_for_host) {
                    ERROR("error interrupt packet send in wrong direction");
                    return 0;
                }
                break;
            case usb_redir_buffered_bulk_packet:
                ERROR("error buffered bulk packet send in wrong direction");
                return 0;
            }
        }
    }

    return 1; /* Verify ok */
}