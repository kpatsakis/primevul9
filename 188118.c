static int usb_parse_endpoint(struct device *ddev, int cfgno, int inum,
    int asnum, struct usb_host_interface *ifp, int num_ep,
    unsigned char *buffer, int size)
{
	unsigned char *buffer0 = buffer;
	struct usb_endpoint_descriptor *d;
	struct usb_host_endpoint *endpoint;
	int n, i, j, retval;
	unsigned int maxp;
	const unsigned short *maxpacket_maxes;

	d = (struct usb_endpoint_descriptor *) buffer;
	buffer += d->bLength;
	size -= d->bLength;

	if (d->bLength >= USB_DT_ENDPOINT_AUDIO_SIZE)
		n = USB_DT_ENDPOINT_AUDIO_SIZE;
	else if (d->bLength >= USB_DT_ENDPOINT_SIZE)
		n = USB_DT_ENDPOINT_SIZE;
	else {
		dev_warn(ddev, "config %d interface %d altsetting %d has an "
		    "invalid endpoint descriptor of length %d, skipping\n",
		    cfgno, inum, asnum, d->bLength);
		goto skip_to_next_endpoint_or_interface_descriptor;
	}

	i = d->bEndpointAddress & ~USB_ENDPOINT_DIR_MASK;
	if (i >= 16 || i == 0) {
		dev_warn(ddev, "config %d interface %d altsetting %d has an "
		    "invalid endpoint with address 0x%X, skipping\n",
		    cfgno, inum, asnum, d->bEndpointAddress);
		goto skip_to_next_endpoint_or_interface_descriptor;
	}

	/* Only store as many endpoints as we have room for */
	if (ifp->desc.bNumEndpoints >= num_ep)
		goto skip_to_next_endpoint_or_interface_descriptor;

	/* Check for duplicate endpoint addresses */
	for (i = 0; i < ifp->desc.bNumEndpoints; ++i) {
		if (ifp->endpoint[i].desc.bEndpointAddress ==
		    d->bEndpointAddress) {
			dev_warn(ddev, "config %d interface %d altsetting %d has a duplicate endpoint with address 0x%X, skipping\n",
			    cfgno, inum, asnum, d->bEndpointAddress);
			goto skip_to_next_endpoint_or_interface_descriptor;
		}
	}

	endpoint = &ifp->endpoint[ifp->desc.bNumEndpoints];
	++ifp->desc.bNumEndpoints;

	memcpy(&endpoint->desc, d, n);
	INIT_LIST_HEAD(&endpoint->urb_list);

	/*
	 * Fix up bInterval values outside the legal range.
	 * Use 10 or 8 ms if no proper value can be guessed.
	 */
	i = 0;		/* i = min, j = max, n = default */
	j = 255;
	if (usb_endpoint_xfer_int(d)) {
		i = 1;
		switch (to_usb_device(ddev)->speed) {
		case USB_SPEED_SUPER_PLUS:
		case USB_SPEED_SUPER:
		case USB_SPEED_HIGH:
			/*
			 * Many device manufacturers are using full-speed
			 * bInterval values in high-speed interrupt endpoint
			 * descriptors. Try to fix those and fall back to an
			 * 8-ms default value otherwise.
			 */
			n = fls(d->bInterval*8);
			if (n == 0)
				n = 7;	/* 8 ms = 2^(7-1) uframes */
			j = 16;

			/*
			 * Adjust bInterval for quirked devices.
			 */
			/*
			 * This quirk fixes bIntervals reported in ms.
			 */
			if (to_usb_device(ddev)->quirks &
				USB_QUIRK_LINEAR_FRAME_INTR_BINTERVAL) {
				n = clamp(fls(d->bInterval) + 3, i, j);
				i = j = n;
			}
			/*
			 * This quirk fixes bIntervals reported in
			 * linear microframes.
			 */
			if (to_usb_device(ddev)->quirks &
				USB_QUIRK_LINEAR_UFRAME_INTR_BINTERVAL) {
				n = clamp(fls(d->bInterval), i, j);
				i = j = n;
			}
			break;
		default:		/* USB_SPEED_FULL or _LOW */
			/*
			 * For low-speed, 10 ms is the official minimum.
			 * But some "overclocked" devices might want faster
			 * polling so we'll allow it.
			 */
			n = 10;
			break;
		}
	} else if (usb_endpoint_xfer_isoc(d)) {
		i = 1;
		j = 16;
		switch (to_usb_device(ddev)->speed) {
		case USB_SPEED_HIGH:
			n = 7;		/* 8 ms = 2^(7-1) uframes */
			break;
		default:		/* USB_SPEED_FULL */
			n = 4;		/* 8 ms = 2^(4-1) frames */
			break;
		}
	}
	if (d->bInterval < i || d->bInterval > j) {
		dev_warn(ddev, "config %d interface %d altsetting %d "
		    "endpoint 0x%X has an invalid bInterval %d, "
		    "changing to %d\n",
		    cfgno, inum, asnum,
		    d->bEndpointAddress, d->bInterval, n);
		endpoint->desc.bInterval = n;
	}

	/* Some buggy low-speed devices have Bulk endpoints, which is
	 * explicitly forbidden by the USB spec.  In an attempt to make
	 * them usable, we will try treating them as Interrupt endpoints.
	 */
	if (to_usb_device(ddev)->speed == USB_SPEED_LOW &&
			usb_endpoint_xfer_bulk(d)) {
		dev_warn(ddev, "config %d interface %d altsetting %d "
		    "endpoint 0x%X is Bulk; changing to Interrupt\n",
		    cfgno, inum, asnum, d->bEndpointAddress);
		endpoint->desc.bmAttributes = USB_ENDPOINT_XFER_INT;
		endpoint->desc.bInterval = 1;
		if (usb_endpoint_maxp(&endpoint->desc) > 8)
			endpoint->desc.wMaxPacketSize = cpu_to_le16(8);
	}

	/* Validate the wMaxPacketSize field */
	maxp = usb_endpoint_maxp(&endpoint->desc);

	/* Find the highest legal maxpacket size for this endpoint */
	i = 0;		/* additional transactions per microframe */
	switch (to_usb_device(ddev)->speed) {
	case USB_SPEED_LOW:
		maxpacket_maxes = low_speed_maxpacket_maxes;
		break;
	case USB_SPEED_FULL:
		maxpacket_maxes = full_speed_maxpacket_maxes;
		break;
	case USB_SPEED_HIGH:
		/* Bits 12..11 are allowed only for HS periodic endpoints */
		if (usb_endpoint_xfer_int(d) || usb_endpoint_xfer_isoc(d)) {
			i = maxp & (BIT(12) | BIT(11));
			maxp &= ~i;
		}
		/* fallthrough */
	default:
		maxpacket_maxes = high_speed_maxpacket_maxes;
		break;
	case USB_SPEED_SUPER:
	case USB_SPEED_SUPER_PLUS:
		maxpacket_maxes = super_speed_maxpacket_maxes;
		break;
	}
	j = maxpacket_maxes[usb_endpoint_type(&endpoint->desc)];

	if (maxp > j) {
		dev_warn(ddev, "config %d interface %d altsetting %d endpoint 0x%X has invalid maxpacket %d, setting to %d\n",
		    cfgno, inum, asnum, d->bEndpointAddress, maxp, j);
		maxp = j;
		endpoint->desc.wMaxPacketSize = cpu_to_le16(i | maxp);
	}

	/*
	 * Some buggy high speed devices have bulk endpoints using
	 * maxpacket sizes other than 512.  High speed HCDs may not
	 * be able to handle that particular bug, so let's warn...
	 */
	if (to_usb_device(ddev)->speed == USB_SPEED_HIGH
			&& usb_endpoint_xfer_bulk(d)) {
		if (maxp != 512)
			dev_warn(ddev, "config %d interface %d altsetting %d "
				"bulk endpoint 0x%X has invalid maxpacket %d\n",
				cfgno, inum, asnum, d->bEndpointAddress,
				maxp);
	}

	/* Parse a possible SuperSpeed endpoint companion descriptor */
	if (to_usb_device(ddev)->speed >= USB_SPEED_SUPER)
		usb_parse_ss_endpoint_companion(ddev, cfgno,
				inum, asnum, endpoint, buffer, size);

	/* Skip over any Class Specific or Vendor Specific descriptors;
	 * find the next endpoint or interface descriptor */
	endpoint->extra = buffer;
	i = find_next_descriptor(buffer, size, USB_DT_ENDPOINT,
			USB_DT_INTERFACE, &n);
	endpoint->extralen = i;
	retval = buffer - buffer0 + i;
	if (n > 0)
		dev_dbg(ddev, "skipped %d descriptor%s after %s\n",
		    n, plural(n), "endpoint");
	return retval;

skip_to_next_endpoint_or_interface_descriptor:
	i = find_next_descriptor(buffer, size, USB_DT_ENDPOINT,
	    USB_DT_INTERFACE, NULL);
	return buffer - buffer0 + i;
}