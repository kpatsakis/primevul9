static inline struct usb_function_instance *to_usb_function_instance(
		struct config_item *item)
{
	 return container_of(to_config_group(item),
			 struct usb_function_instance, group);
}