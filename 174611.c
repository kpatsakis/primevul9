static void swap_bytes(u8 *b, int length)
{
	length -= length % 2;
	for (; length; b += 2, length -= 2)
		swap(*b, *(b + 1));
}