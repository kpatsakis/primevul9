CACHE_LIMITER_FUNC(nocache) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");

	/* For HTTP/1.1 conforming clients */
	ADD_HEADER("Cache-Control: no-store, no-cache, must-revalidate");

	/* For HTTP/1.0 conforming clients */
	ADD_HEADER("Pragma: no-cache");
}