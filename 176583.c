static inline void crst_table_init(unsigned long *crst, unsigned long entry)
{
	clear_table(crst, entry, sizeof(unsigned long)*2048);
}