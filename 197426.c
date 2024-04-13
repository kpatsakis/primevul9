s32 i2c_smbus_write_byte(const struct i2c_client *client, u8 value)
{
	return i2c_smbus_xfer(client->adapter, client->addr, client->flags,
	                      I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE, NULL);
}