static inline time_t FileTimeToUnixTime(const FILETIME FileTime)
{
	__int64 UnixTime;
	long *nsec = NULL;
	SYSTEMTIME SystemTime;
	FileTimeToSystemTime(&FileTime, &SystemTime);

	UnixTime = ((__int64)FileTime.dwHighDateTime << 32) +
	FileTime.dwLowDateTime;

	UnixTime -= (SECS_BETWEEN_EPOCHS * SECS_TO_100NS);

	if (nsec) {
		*nsec = (UnixTime % SECS_TO_100NS) * (__int64)100;
	}

	UnixTime /= SECS_TO_100NS; /* now convert to seconds */

	if ((time_t)UnixTime != UnixTime) {
		UnixTime = 0;
	}
	return (time_t)UnixTime;
}