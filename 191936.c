dump_index(const KeyWord *k, const int *index)
{
	int			i,
				count = 0,
				free_i = 0;

	elog(DEBUG_elog_output, "TO-FROM_CHAR: Dump KeyWord Index:");

	for (i = 0; i < KeyWord_INDEX_SIZE; i++)
	{
		if (index[i] != -1)
		{
			elog(DEBUG_elog_output, "\t%c: %s, ", i + 32, k[index[i]].name);
			count++;
		}
		else
		{
			free_i++;
			elog(DEBUG_elog_output, "\t(%d) %c %d", i, i + 32, index[i]);
		}
	}
	elog(DEBUG_elog_output, "\n\t\tUsed positions: %d,\n\t\tFree positions: %d",
		 count, free_i);
}