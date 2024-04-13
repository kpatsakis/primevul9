read_next_symbol(struct archive_read *a, struct huffman_code *code)
{
  unsigned char bit;
  unsigned int bits;
  int length, value, node;
  struct rar *rar;
  struct rar_br *br;

  if (!code->table)
  {
    if (make_table(a, code) != (ARCHIVE_OK))
      return -1;
  }

  rar = (struct rar *)(a->format->data);
  br = &(rar->br);

  /* Look ahead (peek) at bits */
  if (!rar_br_read_ahead(a, br, code->tablesize)) {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "Truncated RAR file data");
    rar->valid = 0;
    return -1;
  }
  bits = rar_br_bits(br, code->tablesize);

  length = code->table[bits].length;
  value = code->table[bits].value;

  if (length < 0)
  {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "Invalid prefix code in bitstream");
    return -1;
  }

  if (length <= code->tablesize)
  {
    /* Skip length bits */
    rar_br_consume(br, length);
    return value;
  }

  /* Skip tablesize bits */
  rar_br_consume(br, code->tablesize);

  node = value;
  while (!(code->tree[node].branches[0] ==
    code->tree[node].branches[1]))
  {
    if (!rar_br_read_ahead(a, br, 1)) {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                        "Truncated RAR file data");
      rar->valid = 0;
      return -1;
    }
    bit = rar_br_bits(br, 1);
    rar_br_consume(br, 1);

    if (code->tree[node].branches[bit] < 0)
    {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                        "Invalid prefix code in bitstream");
      return -1;
    }
    node = code->tree[node].branches[bit];
  }

  return code->tree[node].branches[0];
}