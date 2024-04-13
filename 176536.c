static int i740fb_setup_ddc_bus(struct fb_info *info)
{
	struct i740fb_par *par = info->par;

	strlcpy(par->ddc_adapter.name, info->fix.id,
		sizeof(par->ddc_adapter.name));
	par->ddc_adapter.owner		= THIS_MODULE;
	par->ddc_adapter.class		= I2C_CLASS_DDC;
	par->ddc_adapter.algo_data	= &par->ddc_algo;
	par->ddc_adapter.dev.parent	= info->device;
	par->ddc_algo.setsda		= i740fb_ddc_setsda;
	par->ddc_algo.setscl		= i740fb_ddc_setscl;
	par->ddc_algo.getsda		= i740fb_ddc_getsda;
	par->ddc_algo.getscl		= i740fb_ddc_getscl;
	par->ddc_algo.udelay		= 10;
	par->ddc_algo.timeout		= 20;
	par->ddc_algo.data		= par;

	i2c_set_adapdata(&par->ddc_adapter, par);

	return i2c_bit_add_bus(&par->ddc_adapter);
}