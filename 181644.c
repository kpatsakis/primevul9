KaxBlockVirtual::~KaxBlockVirtual()
{
  if(GetBuffer() == DataBlock)
    SetBuffer( NULL, 0 );
}