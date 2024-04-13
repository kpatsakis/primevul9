static int configfs_composite_bind(struct usb_gadget *gadget,
		struct usb_gadget_driver *gdriver)
{
	struct usb_composite_driver     *composite = to_cdriver(gdriver);
	struct gadget_info		*gi = container_of(composite,
						struct gadget_info, composite);
	struct usb_composite_dev	*cdev = &gi->cdev;
	struct usb_configuration	*c;
	struct usb_string		*s;
	unsigned			i;
	int				ret;

	/* the gi->lock is hold by the caller */
	gi->unbind = 0;
	cdev->gadget = gadget;
	set_gadget_data(gadget, cdev);
	ret = composite_dev_prepare(composite, cdev);
	if (ret)
		return ret;
	/* and now the gadget bind */
	ret = -EINVAL;

	if (list_empty(&gi->cdev.configs)) {
		pr_err("Need at least one configuration in %s.\n",
				gi->composite.name);
		goto err_comp_cleanup;
	}


	list_for_each_entry(c, &gi->cdev.configs, list) {
		struct config_usb_cfg *cfg;

		cfg = container_of(c, struct config_usb_cfg, c);
		if (list_empty(&cfg->func_list)) {
			pr_err("Config %s/%d of %s needs at least one function.\n",
			      c->label, c->bConfigurationValue,
			      gi->composite.name);
			goto err_comp_cleanup;
		}
	}

	/* init all strings */
	if (!list_empty(&gi->string_list)) {
		struct gadget_strings *gs;

		i = 0;
		list_for_each_entry(gs, &gi->string_list, list) {

			gi->gstrings[i] = &gs->stringtab_dev;
			gs->stringtab_dev.strings = gs->strings;
			gs->strings[USB_GADGET_MANUFACTURER_IDX].s =
				gs->manufacturer;
			gs->strings[USB_GADGET_PRODUCT_IDX].s = gs->product;
			gs->strings[USB_GADGET_SERIAL_IDX].s = gs->serialnumber;
			i++;
		}
		gi->gstrings[i] = NULL;
		s = usb_gstrings_attach(&gi->cdev, gi->gstrings,
				USB_GADGET_FIRST_AVAIL_IDX);
		if (IS_ERR(s)) {
			ret = PTR_ERR(s);
			goto err_comp_cleanup;
		}

		gi->cdev.desc.iManufacturer = s[USB_GADGET_MANUFACTURER_IDX].id;
		gi->cdev.desc.iProduct = s[USB_GADGET_PRODUCT_IDX].id;
		gi->cdev.desc.iSerialNumber = s[USB_GADGET_SERIAL_IDX].id;
	}

	if (gi->use_os_desc) {
		cdev->use_os_string = true;
		cdev->b_vendor_code = gi->b_vendor_code;
		memcpy(cdev->qw_sign, gi->qw_sign, OS_STRING_QW_SIGN_LEN);
	}

	if (gadget_is_otg(gadget) && !otg_desc[0]) {
		struct usb_descriptor_header *usb_desc;

		usb_desc = usb_otg_descriptor_alloc(gadget);
		if (!usb_desc) {
			ret = -ENOMEM;
			goto err_comp_cleanup;
		}
		usb_otg_descriptor_init(gadget, usb_desc);
		otg_desc[0] = usb_desc;
		otg_desc[1] = NULL;
	}

	/* Go through all configs, attach all functions */
	list_for_each_entry(c, &gi->cdev.configs, list) {
		struct config_usb_cfg *cfg;
		struct usb_function *f;
		struct usb_function *tmp;
		struct gadget_config_name *cn;

		if (gadget_is_otg(gadget))
			c->descriptors = otg_desc;

		cfg = container_of(c, struct config_usb_cfg, c);
		if (!list_empty(&cfg->string_list)) {
			i = 0;
			list_for_each_entry(cn, &cfg->string_list, list) {
				cfg->gstrings[i] = &cn->stringtab_dev;
				cn->stringtab_dev.strings = &cn->strings;
				cn->strings.s = cn->configuration;
				i++;
			}
			cfg->gstrings[i] = NULL;
			s = usb_gstrings_attach(&gi->cdev, cfg->gstrings, 1);
			if (IS_ERR(s)) {
				ret = PTR_ERR(s);
				goto err_comp_cleanup;
			}
			c->iConfiguration = s[0].id;
		}

		list_for_each_entry_safe(f, tmp, &cfg->func_list, list) {
			list_del(&f->list);
			ret = usb_add_function(c, f);
			if (ret) {
				list_add(&f->list, &cfg->func_list);
				goto err_purge_funcs;
			}
		}
		usb_ep_autoconfig_reset(cdev->gadget);
	}
	if (cdev->use_os_string) {
		ret = composite_os_desc_req_prepare(cdev, gadget->ep0);
		if (ret)
			goto err_purge_funcs;
	}

	usb_ep_autoconfig_reset(cdev->gadget);
	return 0;

err_purge_funcs:
	purge_configs_funcs(gi);
err_comp_cleanup:
	composite_dev_cleanup(cdev);
	return ret;
}