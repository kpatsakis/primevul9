prefix_uli(uint8_t *data, size_t size)
{
	size_t i = 0;

	if (i < size && data[i] == ' ') i++;
	if (i < size && data[i] == ' ') i++;
	if (i < size && data[i] == ' ') i++;

	if (i + 1 >= size ||
		(data[i] != '*' && data[i] != '+' && data[i] != '-') ||
		data[i + 1] != ' ')
		return 0;

	if (is_next_headerline(data + i, size - i))
		return 0;

	return i + 2;
}