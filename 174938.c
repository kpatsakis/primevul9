lzss_emit_literal(struct rar *rar, uint8_t literal)
{
  *lzss_current_pointer(&rar->lzss) = literal;
  rar->lzss.position++;
}