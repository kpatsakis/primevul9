static struct config_group *config_desc_make(
		struct config_group *group,
		const char *name)
{
	struct gadget_info *gi;
	struct config_usb_cfg *cfg;
	char buf[MAX_NAME_LEN];
	char *num_str;
	u8 num;
	int ret;

	gi = container_of(group, struct gadget_info, configs_group);
	ret = snprintf(buf, MAX_NAME_LEN, "%s", name);
	if (ret >= MAX_NAME_LEN)
		return ERR_PTR(-ENAMETOOLONG);

	num_str = strchr(buf, '.');
	if (!num_str) {
		pr_err("Unable to locate . in name.bConfigurationValue\n");
		return ERR_PTR(-EINVAL);
	}

	*num_str = '\0';
	num_str++;

	if (!strlen(buf))
		return ERR_PTR(-EINVAL);

	ret = kstrtou8(num_str, 0, &num);
	if (ret)
		return ERR_PTR(ret);

	cfg = kzalloc(sizeof(*cfg), GFP_KERNEL);
	if (!cfg)
		return ERR_PTR(-ENOMEM);
	cfg->c.label = kstrdup(buf, GFP_KERNEL);
	if (!cfg->c.label) {
		ret = -ENOMEM;
		goto err;
	}
	cfg->c.bConfigurationValue = num;
	cfg->c.MaxPower = CONFIG_USB_GADGET_VBUS_DRAW;
	cfg->c.bmAttributes = USB_CONFIG_ATT_ONE;
	INIT_LIST_HEAD(&cfg->string_list);
	INIT_LIST_HEAD(&cfg->func_list);

	config_group_init_type_name(&cfg->group, name,
				&gadget_config_type);

	config_group_init_type_name(&cfg->strings_group, "strings",
			&gadget_config_name_strings_type);
	configfs_add_default_group(&cfg->strings_group, &cfg->group);

	ret = usb_add_config_only(&gi->cdev, &cfg->c);
	if (ret)
		goto err;

	return &cfg->group;
err:
	kfree(cfg->c.label);
	kfree(cfg);
	return ERR_PTR(ret);
}