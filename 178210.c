static int parse_xargs(char *data, int data_length, char ***output)
{
	int num = 0;
	char *cur = data;

	if (!data || *output != NULL)
		return -1;

	while (cur < data + data_length) {
		num++;
		*output = must_realloc(*output, (num + 1) * sizeof(**output));
		(*output)[num - 1] = cur;
		cur += strlen(cur) + 1;
	}
	(*output)[num] = NULL;
	return num;
}