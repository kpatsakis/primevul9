static void usbredirparser_call_type_func(struct usbredirparser *parser_pub,
    bool *data_ownership_transferred)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    uint64_t id;

    if (usbredirparser_using_32bits_ids(parser_pub))
        id = parser->header_32bit_id.id;
    else
        id = parser->header.id;

    switch (parser->header.type) {
    case usb_redir_hello:
        usbredirparser_handle_hello(parser_pub,
            (struct usb_redir_hello_header *)parser->type_header,
            parser->data, parser->data_len);
        break;
    case usb_redir_device_connect:
        parser->callb.device_connect_func(parser->callb.priv,
            (struct usb_redir_device_connect_header *)parser->type_header);
        break;
    case usb_redir_device_disconnect:
        parser->callb.device_disconnect_func(parser->callb.priv);
        if (usbredirparser_peer_has_cap(parser_pub,
                                        usb_redir_cap_device_disconnect_ack))
            usbredirparser_queue(parser_pub, usb_redir_device_disconnect_ack,
                                 0, NULL, NULL, 0);
        break;
    case usb_redir_reset:
        parser->callb.reset_func(parser->callb.priv);
        break;
    case usb_redir_interface_info:
        parser->callb.interface_info_func(parser->callb.priv,
            (struct usb_redir_interface_info_header *)parser->type_header);
        break;
    case usb_redir_ep_info:
        parser->callb.ep_info_func(parser->callb.priv,
            (struct usb_redir_ep_info_header *)parser->type_header);
        break;
    case usb_redir_set_configuration:
        parser->callb.set_configuration_func(parser->callb.priv, id,
            (struct usb_redir_set_configuration_header *)parser->type_header);
        break;
    case usb_redir_get_configuration:
        parser->callb.get_configuration_func(parser->callb.priv, id);
        break;
    case usb_redir_configuration_status:
        parser->callb.configuration_status_func(parser->callb.priv, id,
          (struct usb_redir_configuration_status_header *)parser->type_header);
        break;
    case usb_redir_set_alt_setting:
        parser->callb.set_alt_setting_func(parser->callb.priv, id,
            (struct usb_redir_set_alt_setting_header *)parser->type_header);
        break;
    case usb_redir_get_alt_setting:
        parser->callb.get_alt_setting_func(parser->callb.priv, id,
            (struct usb_redir_get_alt_setting_header *)parser->type_header);
        break;
    case usb_redir_alt_setting_status:
        parser->callb.alt_setting_status_func(parser->callb.priv, id,
            (struct usb_redir_alt_setting_status_header *)parser->type_header);
        break;
    case usb_redir_start_iso_stream:
        parser->callb.start_iso_stream_func(parser->callb.priv, id,
            (struct usb_redir_start_iso_stream_header *)parser->type_header);
        break;
    case usb_redir_stop_iso_stream:
        parser->callb.stop_iso_stream_func(parser->callb.priv, id,
            (struct usb_redir_stop_iso_stream_header *)parser->type_header);
        break;
    case usb_redir_iso_stream_status:
        parser->callb.iso_stream_status_func(parser->callb.priv, id,
            (struct usb_redir_iso_stream_status_header *)parser->type_header);
        break;
    case usb_redir_start_interrupt_receiving:
        parser->callb.start_interrupt_receiving_func(parser->callb.priv, id,
            (struct usb_redir_start_interrupt_receiving_header *)
            parser->type_header);
        break;
    case usb_redir_stop_interrupt_receiving:
        parser->callb.stop_interrupt_receiving_func(parser->callb.priv, id,
            (struct usb_redir_stop_interrupt_receiving_header *)
            parser->type_header);
        break;
    case usb_redir_interrupt_receiving_status:
        parser->callb.interrupt_receiving_status_func(parser->callb.priv, id,
            (struct usb_redir_interrupt_receiving_status_header *)
            parser->type_header);
        break;
    case usb_redir_alloc_bulk_streams:
        parser->callb.alloc_bulk_streams_func(parser->callb.priv, id,
            (struct usb_redir_alloc_bulk_streams_header *)parser->type_header);
        break;
    case usb_redir_free_bulk_streams:
        parser->callb.free_bulk_streams_func(parser->callb.priv, id,
            (struct usb_redir_free_bulk_streams_header *)parser->type_header);
        break;
    case usb_redir_bulk_streams_status:
        parser->callb.bulk_streams_status_func(parser->callb.priv, id,
          (struct usb_redir_bulk_streams_status_header *)parser->type_header);
        break;
    case usb_redir_cancel_data_packet:
        parser->callb.cancel_data_packet_func(parser->callb.priv, id);
        break;
    case usb_redir_filter_reject:
        parser->callb.filter_reject_func(parser->callb.priv);
        break;
    case usb_redir_filter_filter: {
        struct usbredirfilter_rule *rules;
        int r, count;

        r = usbredirfilter_string_to_rules((char *)parser->data, ",", "|",
                                           &rules, &count);
        if (r) {
            ERROR("error parsing filter (%d), ignoring filter message", r);
            break;
        }
        parser->callb.filter_filter_func(parser->callb.priv, rules, count);
        break;
    }
    case usb_redir_device_disconnect_ack:
        parser->callb.device_disconnect_ack_func(parser->callb.priv);
        break;
    case usb_redir_start_bulk_receiving:
        parser->callb.start_bulk_receiving_func(parser->callb.priv, id,
            (struct usb_redir_start_bulk_receiving_header *)
            parser->type_header);
        break;
    case usb_redir_stop_bulk_receiving:
        parser->callb.stop_bulk_receiving_func(parser->callb.priv, id,
            (struct usb_redir_stop_bulk_receiving_header *)
            parser->type_header);
        break;
    case usb_redir_bulk_receiving_status:
        parser->callb.bulk_receiving_status_func(parser->callb.priv, id,
            (struct usb_redir_bulk_receiving_status_header *)
            parser->type_header);
        break;
    case usb_redir_control_packet:
        *data_ownership_transferred = true;
        parser->callb.control_packet_func(parser->callb.priv, id,
            (struct usb_redir_control_packet_header *)parser->type_header,
            parser->data, parser->data_len);
        break;
    case usb_redir_bulk_packet:
        *data_ownership_transferred = true;
        parser->callb.bulk_packet_func(parser->callb.priv, id,
            (struct usb_redir_bulk_packet_header *)parser->type_header,
            parser->data, parser->data_len);
        break;
    case usb_redir_iso_packet:
        *data_ownership_transferred = true;
        parser->callb.iso_packet_func(parser->callb.priv, id,
            (struct usb_redir_iso_packet_header *)parser->type_header,
            parser->data, parser->data_len);
        break;
    case usb_redir_interrupt_packet:
        *data_ownership_transferred = true;
        parser->callb.interrupt_packet_func(parser->callb.priv, id,
            (struct usb_redir_interrupt_packet_header *)parser->type_header,
            parser->data, parser->data_len);
        break;
    case usb_redir_buffered_bulk_packet:
        *data_ownership_transferred = true;
        parser->callb.buffered_bulk_packet_func(parser->callb.priv, id,
          (struct usb_redir_buffered_bulk_packet_header *)parser->type_header,
          parser->data, parser->data_len);
        break;
    }
}