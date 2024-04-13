void APE::Properties::analyzeOld()
{
  ByteVector header = d->file->readBlock(26);
  uint totalFrames = header.mid(18, 4).toUInt(false);

  // Fail on 0 length APE files (catches non-finalized APE files)
  if(totalFrames == 0)
    return;

  short compressionLevel = header.mid(0, 2).toShort(false);
  uint blocksPerFrame;
  if(d->version >= 3950)
    blocksPerFrame = 73728 * 4;
  else if(d->version >= 3900 || (d->version >= 3800 && compressionLevel == 4000))
    blocksPerFrame = 73728;
  else
    blocksPerFrame = 9216;
  d->channels = header.mid(4, 2).toShort(false);
  d->sampleRate = header.mid(6, 4).toUInt(false);
  uint finalFrameBlocks = header.mid(22, 4).toUInt(false);
  uint totalBlocks = totalFrames > 0 ? (totalFrames - 1) * blocksPerFrame + finalFrameBlocks : 0;
  d->length = totalBlocks / d->sampleRate;
  d->bitrate = d->length > 0 ? ((d->streamLength * 8L) / d->length) / 1000 : 0;
}