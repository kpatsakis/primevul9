static struct config_group *gadgets_make(
		struct config_group *group,
		const char *name)
{
	struct gadget_info *gi;

	gi = kzalloc(sizeof(*gi), GFP_KERNEL);
	if (!gi)
		return ERR_PTR(-ENOMEM);

	config_group_init_type_name(&gi->group, name, &gadget_root_type);

	config_group_init_type_name(&gi->functions_group, "functions",
			&functions_type);
	configfs_add_default_group(&gi->functions_group, &gi->group);

	config_group_init_type_name(&gi->configs_group, "configs",
			&config_desc_type);
	configfs_add_default_group(&gi->configs_group, &gi->group);

	config_group_init_type_name(&gi->strings_group, "strings",
			&gadget_strings_strings_type);
	configfs_add_default_group(&gi->strings_group, &gi->group);

	config_group_init_type_name(&gi->os_desc_group, "os_desc",
			&os_desc_type);
	configfs_add_default_group(&gi->os_desc_group, &gi->group);

	gi->composite.bind = configfs_do_nothing;
	gi->composite.unbind = configfs_do_nothing;
	gi->composite.suspend = NULL;
	gi->composite.resume = NULL;
	gi->composite.max_speed = USB_SPEED_SUPER;

	spin_lock_init(&gi->spinlock);
	mutex_init(&gi->lock);
	INIT_LIST_HEAD(&gi->string_list);
	INIT_LIST_HEAD(&gi->available_func);

	composite_init_dev(&gi->cdev);
	gi->cdev.desc.bLength = USB_DT_DEVICE_SIZE;
	gi->cdev.desc.bDescriptorType = USB_DT_DEVICE;
	gi->cdev.desc.bcdDevice = cpu_to_le16(get_default_bcdDevice());

	gi->composite.gadget_driver = configfs_driver_template;

	gi->composite.gadget_driver.function = kstrdup(name, GFP_KERNEL);
	gi->composite.name = gi->composite.gadget_driver.function;

	if (!gi->composite.gadget_driver.function)
		goto err;

	return &gi->group;
err:
	kfree(gi);
	return ERR_PTR(-ENOMEM);
}