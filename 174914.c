create_code(struct archive_read *a, struct huffman_code *code,
            unsigned char *lengths, int numsymbols, char maxlength)
{
  int i, j, codebits = 0, symbolsleft = numsymbols;

  code->numentries = 0;
  code->numallocatedentries = 0;
  if (new_node(code) < 0) {
    archive_set_error(&a->archive, ENOMEM,
                      "Unable to allocate memory for node data.");
    return (ARCHIVE_FATAL);
  }
  code->numentries = 1;
  code->minlength = INT_MAX;
  code->maxlength = INT_MIN;
  codebits = 0;
  for(i = 1; i <= maxlength; i++)
  {
    for(j = 0; j < numsymbols; j++)
    {
      if (lengths[j] != i) continue;
      if (add_value(a, code, j, codebits, i) != ARCHIVE_OK)
        return (ARCHIVE_FATAL);
      codebits++;
      if (--symbolsleft <= 0) { break; break; }
    }
    codebits <<= 1;
  }
  return (ARCHIVE_OK);
}