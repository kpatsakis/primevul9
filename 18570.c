static BOOL recurse_check_bit(compiler_common *common, sljit_sw bit_index)
{
uint8_t *byte;
uint8_t mask;

SLJIT_ASSERT((bit_index & (sizeof(sljit_sw) - 1)) == 0);

bit_index >>= SLJIT_WORD_SHIFT;

mask = 1 << (bit_index & 0x7);
byte = common->recurse_bitset + (bit_index >> 3);

if (*byte & mask)
  return FALSE;

*byte |= mask;
return TRUE;
}