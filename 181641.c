void KaxBlockBlob::SetBlockDuration(uint64 TimeLength)
{
  if (ReplaceSimpleByGroup())
    Block.group->SetBlockDuration(TimeLength);
}