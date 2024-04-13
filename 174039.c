setCORKopt(int sock, int onOff)
{
#if defined(TCP_CORK)
	setsockopt(sock, SOL_TCP, TCP_CORK, &onOff, sizeof (onOff));
#elif defined(TCP_NOPUSH)
	setsockopt(sock, IPPROTO_TCP, TCP_NOPUSH, &onOff, sizeof (onOff));
#endif
}