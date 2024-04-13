static double php_log1p(double x)
{
#ifdef HAVE_LOG1P
	return(log1p(x));
#else
	return(log(1 + x));
#endif
}