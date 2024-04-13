mp_sint32 PlayerGeneric::beatPacketsToBufferSize(mp_uint32 numBeats)
{
	return ChannelMixer::beatPacketsToBufferSize(getMixFrequency(), numBeats);
}