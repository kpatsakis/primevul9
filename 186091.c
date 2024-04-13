static int of_path_i2c_client_exists(const char *path)
{
	struct i2c_client *client;

	client = of_path_to_i2c_client(path);
	if (client)
		put_device(&client->dev);
	return client != NULL;
}