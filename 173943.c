GetGdtr (
  IN OUT VOID *Buffer
  )
{
  AsmReadGdtr ((IA32_DESCRIPTOR *)Buffer);
}