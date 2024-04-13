char *sigfig(double val, int figs, char *buf, int len)
{
	char format[80];
	int digitsafterdecimal=figs-(int)ceil(log10(fabs(val)));
	if(digitsafterdecimal<1) snprintf(format, 80, "%%.0f");
	else snprintf(format, 80, "%%.%df", digitsafterdecimal);
	snprintf(buf, len, format, val);
	return buf;
}