static void gen_ly(int eob)
{
	generate();
	if (info['W' - 'A']) {
		put_words(info['W' - 'A']);
		info['W' - 'A'] = NULL;
	}
	if (eob)
		buffer_eob(0);
}