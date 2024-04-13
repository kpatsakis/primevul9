bool PlayerGeneric::isInitialized() const
{
	if (mixer)
		return mixer->isInitialized();
		
	return false;
}