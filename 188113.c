static int usb_parse_configuration(struct usb_device *dev, int cfgidx,
    struct usb_host_config *config, unsigned char *buffer, int size)
{
	struct device *ddev = &dev->dev;
	unsigned char *buffer0 = buffer;
	int cfgno;
	int nintf, nintf_orig;
	int i, j, n;
	struct usb_interface_cache *intfc;
	unsigned char *buffer2;
	int size2;
	struct usb_descriptor_header *header;
	int len, retval;
	u8 inums[USB_MAXINTERFACES], nalts[USB_MAXINTERFACES];
	unsigned iad_num = 0;

	memcpy(&config->desc, buffer, USB_DT_CONFIG_SIZE);
	nintf = nintf_orig = config->desc.bNumInterfaces;
	config->desc.bNumInterfaces = 0;	// Adjusted later

	if (config->desc.bDescriptorType != USB_DT_CONFIG ||
	    config->desc.bLength < USB_DT_CONFIG_SIZE ||
	    config->desc.bLength > size) {
		dev_err(ddev, "invalid descriptor for config index %d: "
		    "type = 0x%X, length = %d\n", cfgidx,
		    config->desc.bDescriptorType, config->desc.bLength);
		return -EINVAL;
	}
	cfgno = config->desc.bConfigurationValue;

	buffer += config->desc.bLength;
	size -= config->desc.bLength;

	if (nintf > USB_MAXINTERFACES) {
		dev_warn(ddev, "config %d has too many interfaces: %d, "
		    "using maximum allowed: %d\n",
		    cfgno, nintf, USB_MAXINTERFACES);
		nintf = USB_MAXINTERFACES;
	}

	/* Go through the descriptors, checking their length and counting the
	 * number of altsettings for each interface */
	n = 0;
	for ((buffer2 = buffer, size2 = size);
	      size2 > 0;
	     (buffer2 += header->bLength, size2 -= header->bLength)) {

		if (size2 < sizeof(struct usb_descriptor_header)) {
			dev_warn(ddev, "config %d descriptor has %d excess "
			    "byte%s, ignoring\n",
			    cfgno, size2, plural(size2));
			break;
		}

		header = (struct usb_descriptor_header *) buffer2;
		if ((header->bLength > size2) || (header->bLength < 2)) {
			dev_warn(ddev, "config %d has an invalid descriptor "
			    "of length %d, skipping remainder of the config\n",
			    cfgno, header->bLength);
			break;
		}

		if (header->bDescriptorType == USB_DT_INTERFACE) {
			struct usb_interface_descriptor *d;
			int inum;

			d = (struct usb_interface_descriptor *) header;
			if (d->bLength < USB_DT_INTERFACE_SIZE) {
				dev_warn(ddev, "config %d has an invalid "
				    "interface descriptor of length %d, "
				    "skipping\n", cfgno, d->bLength);
				continue;
			}

			inum = d->bInterfaceNumber;

			if ((dev->quirks & USB_QUIRK_HONOR_BNUMINTERFACES) &&
			    n >= nintf_orig) {
				dev_warn(ddev, "config %d has more interface "
				    "descriptors, than it declares in "
				    "bNumInterfaces, ignoring interface "
				    "number: %d\n", cfgno, inum);
				continue;
			}

			if (inum >= nintf_orig)
				dev_warn(ddev, "config %d has an invalid "
				    "interface number: %d but max is %d\n",
				    cfgno, inum, nintf_orig - 1);

			/* Have we already encountered this interface?
			 * Count its altsettings */
			for (i = 0; i < n; ++i) {
				if (inums[i] == inum)
					break;
			}
			if (i < n) {
				if (nalts[i] < 255)
					++nalts[i];
			} else if (n < USB_MAXINTERFACES) {
				inums[n] = inum;
				nalts[n] = 1;
				++n;
			}

		} else if (header->bDescriptorType ==
				USB_DT_INTERFACE_ASSOCIATION) {
			struct usb_interface_assoc_descriptor *d;

			d = (struct usb_interface_assoc_descriptor *)header;
			if (d->bLength < USB_DT_INTERFACE_ASSOCIATION_SIZE) {
				dev_warn(ddev,
					 "config %d has an invalid interface association descriptor of length %d, skipping\n",
					 cfgno, d->bLength);
				continue;
			}

			if (iad_num == USB_MAXIADS) {
				dev_warn(ddev, "found more Interface "
					       "Association Descriptors "
					       "than allocated for in "
					       "configuration %d\n", cfgno);
			} else {
				config->intf_assoc[iad_num] = d;
				iad_num++;
			}

		} else if (header->bDescriptorType == USB_DT_DEVICE ||
			    header->bDescriptorType == USB_DT_CONFIG)
			dev_warn(ddev, "config %d contains an unexpected "
			    "descriptor of type 0x%X, skipping\n",
			    cfgno, header->bDescriptorType);

	}	/* for ((buffer2 = buffer, size2 = size); ...) */
	size = buffer2 - buffer;
	config->desc.wTotalLength = cpu_to_le16(buffer2 - buffer0);

	if (n != nintf)
		dev_warn(ddev, "config %d has %d interface%s, different from "
		    "the descriptor's value: %d\n",
		    cfgno, n, plural(n), nintf_orig);
	else if (n == 0)
		dev_warn(ddev, "config %d has no interfaces?\n", cfgno);
	config->desc.bNumInterfaces = nintf = n;

	/* Check for missing interface numbers */
	for (i = 0; i < nintf; ++i) {
		for (j = 0; j < nintf; ++j) {
			if (inums[j] == i)
				break;
		}
		if (j >= nintf)
			dev_warn(ddev, "config %d has no interface number "
			    "%d\n", cfgno, i);
	}

	/* Allocate the usb_interface_caches and altsetting arrays */
	for (i = 0; i < nintf; ++i) {
		j = nalts[i];
		if (j > USB_MAXALTSETTING) {
			dev_warn(ddev, "too many alternate settings for "
			    "config %d interface %d: %d, "
			    "using maximum allowed: %d\n",
			    cfgno, inums[i], j, USB_MAXALTSETTING);
			nalts[i] = j = USB_MAXALTSETTING;
		}

		len = sizeof(*intfc) + sizeof(struct usb_host_interface) * j;
		config->intf_cache[i] = intfc = kzalloc(len, GFP_KERNEL);
		if (!intfc)
			return -ENOMEM;
		kref_init(&intfc->ref);
	}

	/* FIXME: parse the BOS descriptor */

	/* Skip over any Class Specific or Vendor Specific descriptors;
	 * find the first interface descriptor */
	config->extra = buffer;
	i = find_next_descriptor(buffer, size, USB_DT_INTERFACE,
	    USB_DT_INTERFACE, &n);
	config->extralen = i;
	if (n > 0)
		dev_dbg(ddev, "skipped %d descriptor%s after %s\n",
		    n, plural(n), "configuration");
	buffer += i;
	size -= i;

	/* Parse all the interface/altsetting descriptors */
	while (size > 0) {
		retval = usb_parse_interface(ddev, cfgno, config,
		    buffer, size, inums, nalts);
		if (retval < 0)
			return retval;

		buffer += retval;
		size -= retval;
	}

	/* Check for missing altsettings */
	for (i = 0; i < nintf; ++i) {
		intfc = config->intf_cache[i];
		for (j = 0; j < intfc->num_altsetting; ++j) {
			for (n = 0; n < intfc->num_altsetting; ++n) {
				if (intfc->altsetting[n].desc.
				    bAlternateSetting == j)
					break;
			}
			if (n >= intfc->num_altsetting)
				dev_warn(ddev, "config %d interface %d has no "
				    "altsetting %d\n", cfgno, inums[i], j);
		}
	}

	return 0;
}