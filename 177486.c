prefix_code(uint8_t *data, size_t size)
{
	if (size > 3 && data[0] == ' ' && data[1] == ' '
		&& data[2] == ' ' && data[3] == ' ') return 4;

	return 0;
}