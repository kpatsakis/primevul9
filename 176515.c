static void i740fb_ddc_setscl(void *data, int val)
{
	struct i740fb_par *par = data;

	i740outreg_mask(par, XRX, REG_DDC_DRIVE, DDC_SCL, DDC_SCL);
	i740outreg_mask(par, XRX, REG_DDC_STATE, val ? DDC_SCL : 0, DDC_SCL);
}