static void _print_next_block(int idx, const char *blk)
{
	pr_cont("%s%s", idx ? ", " : "", blk);
}