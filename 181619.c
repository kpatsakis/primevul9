KaxBlockVirtual::KaxBlockVirtual(EBML_EXTRA_DEF)
  :EBML_DEF_BINARY(KaxBlockVirtual)EBML_DEF_SEP ParentCluster(NULL)
{
  SetBuffer(DataBlock,sizeof(DataBlock));
  SetValueIsSet(false);
}