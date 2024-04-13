KaxBlockVirtual::KaxBlockVirtual(const KaxBlockVirtual & ElementToClone)
  :EbmlBinary(ElementToClone)
  ,Timecode(ElementToClone.Timecode)
  ,TrackNumber(ElementToClone.TrackNumber)
  ,ParentCluster(ElementToClone.ParentCluster) ///< \todo not exactly
{
  SetBuffer(DataBlock,sizeof(DataBlock));
  SetValueIsSet(false);
}