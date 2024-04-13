getLastTableList ()
{
  if (lastTrans == NULL)
    return NULL;
  strncpy (scratchBuf, lastTrans->tableList, lastTrans->tableListLength);
  scratchBuf[lastTrans->tableListLength] = 0;
  return scratchBuf;
}