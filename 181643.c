KaxInternalBlock::KaxInternalBlock(const KaxInternalBlock & ElementToClone)
  :EbmlBinary(ElementToClone)
  ,myBuffers(ElementToClone.myBuffers.size())
  ,Timecode(ElementToClone.Timecode)
  ,LocalTimecode(ElementToClone.LocalTimecode)
  ,bLocalTimecodeUsed(ElementToClone.bLocalTimecodeUsed)
  ,TrackNumber(ElementToClone.TrackNumber)
  ,ParentCluster(ElementToClone.ParentCluster) ///< \todo not exactly
{
  // add a clone of the list
  std::vector<DataBuffer *>::const_iterator Itr = ElementToClone.myBuffers.begin();
  std::vector<DataBuffer *>::iterator myItr = myBuffers.begin();
  while (Itr != ElementToClone.myBuffers.end()) {
    *myItr = (*Itr)->Clone();
    ++Itr; ++myItr;
  }
}