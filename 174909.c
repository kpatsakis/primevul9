make_table(struct archive_read *a, struct huffman_code *code)
{
  if (code->maxlength < code->minlength || code->maxlength > 10)
    code->tablesize = 10;
  else
    code->tablesize = code->maxlength;

  code->table =
    (struct huffman_table_entry *)calloc(1, sizeof(*code->table)
    * ((size_t)1 << code->tablesize));

  return make_table_recurse(a, code, 0, code->table, 0, code->tablesize);
}