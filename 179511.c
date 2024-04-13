int64_to_str_back(char *ptr, gint64 value)
{
	if (value < 0) {
		ptr = uint64_to_str_back(ptr, -value);
		*(--ptr) = '-';
	} else
		ptr = uint64_to_str_back(ptr, value);

	return ptr;
}