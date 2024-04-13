static void purge_configs_funcs(struct gadget_info *gi)
{
	struct usb_configuration	*c;

	list_for_each_entry(c, &gi->cdev.configs, list) {
		struct usb_function *f, *tmp;
		struct config_usb_cfg *cfg;

		cfg = container_of(c, struct config_usb_cfg, c);

		list_for_each_entry_safe(f, tmp, &c->functions, list) {

			list_move_tail(&f->list, &cfg->func_list);
			if (f->unbind) {
				dev_dbg(&gi->cdev.gadget->dev,
					"unbind function '%s'/%p\n",
					f->name, f);
				f->unbind(c, f);
			}
		}
		c->next_interface_id = 0;
		memset(c->interface, 0, sizeof(c->interface));
		c->superspeed_plus = 0;
		c->superspeed = 0;
		c->highspeed = 0;
		c->fullspeed = 0;
	}
}