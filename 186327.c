CACHE_LIMITER_FUNC(private) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");
	CACHE_LIMITER(private_no_expire)();
}