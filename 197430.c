int of_i2c_setup_smbus_alert(struct i2c_adapter *adapter)
{
	struct i2c_client *client;
	int irq;

	irq = of_property_match_string(adapter->dev.of_node, "interrupt-names",
				       "smbus_alert");
	if (irq == -EINVAL || irq == -ENODATA)
		return 0;
	else if (irq < 0)
		return irq;

	client = i2c_setup_smbus_alert(adapter, NULL);
	if (!client)
		return -ENODEV;

	return 0;
}