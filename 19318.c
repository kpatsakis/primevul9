vreader_slot (int vrdr)
{
  if (vrdr == -1 || !(vrdr >= 0 && vrdr < DIM(vreader_table)))
    return -1;
  if (!vreader_table [vrdr].valid)
    return -1;
  return vreader_table[vrdr].slot;
}