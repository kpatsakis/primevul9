static void i740fb_ddc_setsda(void *data, int val)
{
	struct i740fb_par *par = data;

	i740outreg_mask(par, XRX, REG_DDC_DRIVE, DDC_SDA, DDC_SDA);
	i740outreg_mask(par, XRX, REG_DDC_STATE, val ? DDC_SDA : 0, DDC_SDA);
}