static int unittest_i2c_master_xfer(struct i2c_adapter *adap,
		struct i2c_msg *msgs, int num)
{
	struct unittest_i2c_bus_data *std = i2c_get_adapdata(adap);

	(void)std;

	return num;
}