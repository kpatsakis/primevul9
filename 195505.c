static void usb_ohci_init(OHCIState *ohci, DeviceState *dev,
                          int num_ports, dma_addr_t localmem_base,
                          char *masterbus, uint32_t firstport,
                          AddressSpace *as, Error **errp)
{
    Error *err = NULL;
    int i;

    ohci->as = as;

    if (num_ports > OHCI_MAX_PORTS) {
        error_setg(errp, "OHCI num-ports=%d is too big (limit is %d ports)",
                   num_ports, OHCI_MAX_PORTS);
        return;
    }

    if (usb_frame_time == 0) {
#ifdef OHCI_TIME_WARP
        usb_frame_time = NANOSECONDS_PER_SECOND;
        usb_bit_time = NANOSECONDS_PER_SECOND / (USB_HZ / 1000);
#else
        usb_frame_time = NANOSECONDS_PER_SECOND / 1000;
        if (NANOSECONDS_PER_SECOND >= USB_HZ) {
            usb_bit_time = NANOSECONDS_PER_SECOND / USB_HZ;
        } else {
            usb_bit_time = 1;
        }
#endif
        trace_usb_ohci_init_time(usb_frame_time, usb_bit_time);
    }

    ohci->num_ports = num_ports;
    if (masterbus) {
        USBPort *ports[OHCI_MAX_PORTS];
        for(i = 0; i < num_ports; i++) {
            ports[i] = &ohci->rhport[i].port;
        }
        usb_register_companion(masterbus, ports, num_ports,
                               firstport, ohci, &ohci_port_ops,
                               USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL,
                               &err);
        if (err) {
            error_propagate(errp, err);
            return;
        }
    } else {
        usb_bus_new(&ohci->bus, sizeof(ohci->bus), &ohci_bus_ops, dev);
        for (i = 0; i < num_ports; i++) {
            usb_register_port(&ohci->bus, &ohci->rhport[i].port,
                              ohci, i, &ohci_port_ops,
                              USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL);
        }
    }

    memory_region_init_io(&ohci->mem, OBJECT(dev), &ohci_mem_ops,
                          ohci, "ohci", 256);
    ohci->localmem_base = localmem_base;

    ohci->name = object_get_typename(OBJECT(dev));
    usb_packet_init(&ohci->usb_packet);

    ohci->async_td = 0;

    ohci->eof_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL,
                                   ohci_frame_boundary, ohci);
}