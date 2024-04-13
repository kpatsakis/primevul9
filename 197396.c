static int usbredirparser_get_type_header_len(
    struct usbredirparser *parser_pub, int32_t type, int send)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    int command_for_host = 0;

    if (parser->flags & usbredirparser_fl_usb_host) {
        command_for_host = 1;
    }
    if (send) {
        command_for_host = !command_for_host;
    }

    switch (type) {
    case usb_redir_hello:
        return sizeof(struct usb_redir_hello_header);
    case usb_redir_device_connect:
        if (!command_for_host) {
            if (usbredirparser_have_cap(parser_pub,
                                    usb_redir_cap_connect_device_version) &&
                usbredirparser_peer_has_cap(parser_pub,
                                    usb_redir_cap_connect_device_version)) {
                return sizeof(struct usb_redir_device_connect_header);
            } else {
                return sizeof(struct usb_redir_device_connect_header_no_device_version);
            }
        } else {
            return -1;
        }
    case usb_redir_device_disconnect:
        if (!command_for_host) {
            return 0;
        } else {
            return -1;
        }
    case usb_redir_reset:
        if (command_for_host) {
            return 0; /* No packet type specific header */
        } else {
            return -1;
        }
    case usb_redir_interface_info:
        if (!command_for_host) {
            return sizeof(struct usb_redir_interface_info_header);
        } else {
            return -1;
        }
    case usb_redir_ep_info:
        if (!command_for_host) {
            if (usbredirparser_have_cap(parser_pub,
                                    usb_redir_cap_bulk_streams) &&
                usbredirparser_peer_has_cap(parser_pub,
                                    usb_redir_cap_bulk_streams)) {
                return sizeof(struct usb_redir_ep_info_header);
            } else if (usbredirparser_have_cap(parser_pub,
                                    usb_redir_cap_ep_info_max_packet_size) &&
                       usbredirparser_peer_has_cap(parser_pub,
                                    usb_redir_cap_ep_info_max_packet_size)) {
                return sizeof(struct usb_redir_ep_info_header_no_max_streams);
            } else {
                return sizeof(struct usb_redir_ep_info_header_no_max_pktsz);
            }
        } else {
            return -1;
        }
    case usb_redir_set_configuration:
        if (command_for_host) {
            return sizeof(struct usb_redir_set_configuration_header);
        } else {
            return -1; /* Should never be send to a guest */
        }
    case usb_redir_get_configuration:
        if (command_for_host) {
            return 0; /* No packet type specific header */
        } else {
            return -1;
        }
    case usb_redir_configuration_status:
        if (!command_for_host) {
            return sizeof(struct usb_redir_configuration_status_header);
        } else {
            return -1;
        }
    case usb_redir_set_alt_setting:
        if (command_for_host) {
            return sizeof(struct usb_redir_set_alt_setting_header);
        } else {
            return -1;
        }
    case usb_redir_get_alt_setting:
        if (command_for_host) {
            return sizeof(struct usb_redir_get_alt_setting_header);
        } else {
            return -1;
        }
    case usb_redir_alt_setting_status:
        if (!command_for_host) {
            return sizeof(struct usb_redir_alt_setting_status_header);
        } else {
            return -1;
        }
    case usb_redir_start_iso_stream:
        if (command_for_host) {
            return sizeof(struct usb_redir_start_iso_stream_header);
        } else {
            return -1;
        }
    case usb_redir_stop_iso_stream:
        if (command_for_host) {
            return sizeof(struct usb_redir_stop_iso_stream_header);
        } else {
            return -1;
        }
    case usb_redir_iso_stream_status:
        if (!command_for_host) {
            return sizeof(struct usb_redir_iso_stream_status_header);
        } else {
            return -1;
        }
    case usb_redir_start_interrupt_receiving:
        if (command_for_host) {
            return sizeof(struct usb_redir_start_interrupt_receiving_header);
        } else {
            return -1;
        }
    case usb_redir_stop_interrupt_receiving:
        if (command_for_host) {
            return sizeof(struct usb_redir_stop_interrupt_receiving_header);
        } else {
            return -1;
        }
    case usb_redir_interrupt_receiving_status:
        if (!command_for_host) {
            return sizeof(struct usb_redir_interrupt_receiving_status_header);
        } else {
            return -1;
        }
    case usb_redir_alloc_bulk_streams:
        if (command_for_host) {
            return sizeof(struct usb_redir_alloc_bulk_streams_header);
        } else {
            return -1;
        }
    case usb_redir_free_bulk_streams:
        if (command_for_host) {
            return sizeof(struct usb_redir_free_bulk_streams_header);
        } else {
            return -1;
        }
    case usb_redir_bulk_streams_status:
        if (!command_for_host) {
            return sizeof(struct usb_redir_bulk_streams_status_header);
        } else {
            return -1;
        }
    case usb_redir_cancel_data_packet:
        if (command_for_host) {
            return 0; /* No packet type specific header */
        } else {
            return -1;
        }
    case usb_redir_filter_reject:
        if (command_for_host) {
            return 0;
        } else {
            return -1;
        }
    case usb_redir_filter_filter:
        return 0;
    case usb_redir_device_disconnect_ack:
        if (command_for_host) {
            return 0;
        } else {
            return -1;
        }
    case usb_redir_start_bulk_receiving:
        if (command_for_host) {
            return sizeof(struct usb_redir_start_bulk_receiving_header);
        } else {
            return -1;
        }
    case usb_redir_stop_bulk_receiving:
        if (command_for_host) {
            return sizeof(struct usb_redir_stop_bulk_receiving_header);
        } else {
            return -1;
        }
    case usb_redir_bulk_receiving_status:
        if (!command_for_host) {
            return sizeof(struct usb_redir_bulk_receiving_status_header);
        } else {
            return -1;
        }
    case usb_redir_control_packet:
        return sizeof(struct usb_redir_control_packet_header);
    case usb_redir_bulk_packet:
        if (usbredirparser_have_cap(parser_pub,
                                usb_redir_cap_32bits_bulk_length) &&
            usbredirparser_peer_has_cap(parser_pub,
                                usb_redir_cap_32bits_bulk_length)) {
            return sizeof(struct usb_redir_bulk_packet_header);
        } else {
            return sizeof(struct usb_redir_bulk_packet_header_16bit_length);
        }
    case usb_redir_iso_packet:
        return sizeof(struct usb_redir_iso_packet_header);
    case usb_redir_interrupt_packet:
        return sizeof(struct usb_redir_interrupt_packet_header);
    case usb_redir_buffered_bulk_packet:
        if (!command_for_host) {
            return sizeof(struct usb_redir_buffered_bulk_packet_header);
        } else {
            return -1;
        }
    default:
        return -1;
    }
}