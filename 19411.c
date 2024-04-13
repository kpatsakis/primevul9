bool PlayerGeneric::isPaused() const
{
	if (mixer)
		return mixer->isPaused();
		
	return paused;
}