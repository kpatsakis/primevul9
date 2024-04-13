bool PlayerGeneric::isPlaying() const
{
	if (mixer)
		return mixer->isPlaying();
		
	return false;
}