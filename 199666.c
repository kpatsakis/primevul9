void CDCCBounce::Connected() {
	SetTimeout(0);
	DEBUG(GetSockName() << " == Connected()");
}