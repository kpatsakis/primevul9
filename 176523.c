static int i740fb_ddc_getscl(void *data)
{
	struct i740fb_par *par = data;

	i740outreg_mask(par, XRX, REG_DDC_DRIVE, 0, DDC_SCL);

	return !!(i740inreg(par, XRX, REG_DDC_STATE) & DDC_SCL);
}