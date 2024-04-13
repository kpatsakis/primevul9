static int handle_generic_errors(int rc, const char *msg, ...)
{
	va_list va;

	va_start(va, msg);
	errno = -rc;

	switch(errno) {
	case EINVAL:
	case EPERM:
		vwarn(msg, va);
		rc = MOUNT_EX_USAGE;
		break;
	case ENOMEM:
		vwarn(msg, va);
		rc = MOUNT_EX_SYSERR;
		break;
	default:
		vwarn(msg, va);
		rc = MOUNT_EX_FAIL;
		break;
	}
	va_end(va);
	return rc;
}