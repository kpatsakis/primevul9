static double php_asinh(double z)
{
#ifdef HAVE_ASINH
	return(asinh(z));
#else
	return(log(z + sqrt(1 + pow(z, 2))) / log(M_E));
#endif
}