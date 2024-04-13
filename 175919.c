static int* get_position(int dimension, const char* str)
{
	int *pos;

	pos = safe_emalloc(sizeof(int), dimension, 0);
	get_position_ex(dimension, str, &pos);
	return pos;
}