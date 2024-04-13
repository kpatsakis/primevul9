mp_sint32 PlayerGeneric::adjustBufferSize(mp_uint32 numBeats)
{
	return setBufferSize(beatPacketsToBufferSize(numBeats));
}