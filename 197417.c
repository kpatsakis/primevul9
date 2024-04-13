s32 i2c_smbus_read_block_data(const struct i2c_client *client, u8 command,
			      u8 *values)
{
	union i2c_smbus_data data;
	int status;

	status = i2c_smbus_xfer(client->adapter, client->addr, client->flags,
				I2C_SMBUS_READ, command,
				I2C_SMBUS_BLOCK_DATA, &data);
	if (status)
		return status;

	memcpy(values, &data.block[1], data.block[0]);
	return data.block[0];
}