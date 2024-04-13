static double php_expm1(double x)
{
#if !defined(PHP_WIN32) && !defined(NETWARE)
	return(expm1(x));
#else
	return(exp(x) - 1);
#endif
}