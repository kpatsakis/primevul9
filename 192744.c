static double php_acosh(double x)
{
#ifdef HAVE_ACOSH
	return(acosh(x));
#else
	return(log(x + sqrt(x * x - 1)));
#endif
}