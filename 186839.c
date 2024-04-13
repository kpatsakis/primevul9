static LZWInfo *AcquireLZWInfo(Image *image,const size_t data_size)
{
  LZWInfo
    *lzw_info;

  register ssize_t
    i;

  size_t
    one;

  lzw_info=(LZWInfo *) AcquireMagickMemory(sizeof(*lzw_info));
  if (lzw_info == (LZWInfo *) NULL)
    return((LZWInfo *) NULL);
  (void) ResetMagickMemory(lzw_info,0,sizeof(*lzw_info));
  lzw_info->image=image;
  lzw_info->data_size=data_size;
  one=1;
  lzw_info->maximum_data_value=(one << data_size)-1;
  lzw_info->clear_code=lzw_info->maximum_data_value+1;
  lzw_info->end_code=lzw_info->maximum_data_value+2;
  lzw_info->table[0]=(size_t *) AcquireQuantumMemory(MaximumLZWCode,
    sizeof(**lzw_info->table));
  lzw_info->table[1]=(size_t *) AcquireQuantumMemory(MaximumLZWCode,
    sizeof(**lzw_info->table));
  if ((lzw_info->table[0] == (size_t *) NULL) ||
      (lzw_info->table[1] == (size_t *) NULL))
    {
      lzw_info=RelinquishLZWInfo(lzw_info);
      return((LZWInfo *) NULL);
    }
  for (i=0; i <= (ssize_t) lzw_info->maximum_data_value; i++)
  {
    lzw_info->table[0][i]=0;
    lzw_info->table[1][i]=(size_t) i;
  }
  ResetLZWInfo(lzw_info);
  lzw_info->code_info.buffer[0]='\0';
  lzw_info->code_info.buffer[1]='\0';
  lzw_info->code_info.count=2;
  lzw_info->code_info.bit=8*lzw_info->code_info.count;
  lzw_info->code_info.eof=MagickFalse;
  lzw_info->genesis=MagickTrue;
  lzw_info->stack=(LZWStack *) AcquireMagickMemory(sizeof(*lzw_info->stack));
  if (lzw_info->stack == (LZWStack *) NULL)
    {
      lzw_info=RelinquishLZWInfo(lzw_info);
      return((LZWInfo *) NULL);
    }
  lzw_info->stack->codes=(size_t *) AcquireQuantumMemory(2UL*
    MaximumLZWCode,sizeof(*lzw_info->stack->codes));
  if (lzw_info->stack->codes == (size_t *) NULL)
    {
      lzw_info=RelinquishLZWInfo(lzw_info);
      return((LZWInfo *) NULL);
    }
  lzw_info->stack->index=lzw_info->stack->codes;
  lzw_info->stack->top=lzw_info->stack->codes+2*MaximumLZWCode;
  return(lzw_info);
}