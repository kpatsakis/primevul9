coolkey_make_cuid_from_cplc(coolkey_cuid_t *cuid, global_platform_cplc_data_t *cplc_data)
{
	cuid->ic_fabricator[0]    = cplc_data->ic_fabricator[0];
	cuid->ic_fabricator[1]    = cplc_data->ic_fabricator[1];
	cuid->ic_type[0]          = cplc_data->ic_type[0];
	cuid->ic_type[1]          = cplc_data->ic_type[1];
	cuid->ic_batch[0]         = cplc_data->ic_batch[0];
	cuid->ic_batch[1]         = cplc_data->ic_batch[1];
	cuid->ic_serial_number[0] = cplc_data->ic_serial_number[0];
	cuid->ic_serial_number[1] = cplc_data->ic_serial_number[1];
	cuid->ic_serial_number[2] = cplc_data->ic_serial_number[2];
	cuid->ic_serial_number[3] = cplc_data->ic_serial_number[3];
}