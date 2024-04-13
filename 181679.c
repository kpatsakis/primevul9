des_init_local(struct php_crypt_extended_data *data)
{
	data->old_rawkey0 = data->old_rawkey1 = 0;
	data->saltbits = 0;
	data->old_salt = 0;

	data->initialized = 1;
}