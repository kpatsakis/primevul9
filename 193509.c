clear_reference_bitmap (guint32 offset, guint32 size)
{
	int idx = (offset >> 24) - 1;
	uintptr_t *rb;
	rb = static_reference_bitmaps [idx];
	offset &= 0xffffff;
	offset /= sizeof (gpointer);
	size /= sizeof (gpointer);
	size += offset;
	/* offset is now the bitmap offset */
	for (; offset < size; ++offset)
		rb [offset / (sizeof (uintptr_t) * 8)] &= ~(1L << (offset & (sizeof (uintptr_t) * 8 -1)));
}