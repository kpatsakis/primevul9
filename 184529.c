protectedStrerror(J9PortLibrary* portLib, void* savedErrno)
{
	return (UDATA) strerror((int) (IDATA) savedErrno);
}