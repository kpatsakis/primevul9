s32 i2c_smbus_read_word_data(const struct i2c_client *client, u8 command)
{
	union i2c_smbus_data data;
	int status;

	status = i2c_smbus_xfer(client->adapter, client->addr, client->flags,
				I2C_SMBUS_READ, command,
				I2C_SMBUS_WORD_DATA, &data);
	return (status < 0) ? status : data.word;
}