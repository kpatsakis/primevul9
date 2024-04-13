static double php_atanh(double z)
{
#ifdef HAVE_ATANH
	return(atanh(z));
#else
	return(0.5 * log((1 + z) / (1 - z)));
#endif
}