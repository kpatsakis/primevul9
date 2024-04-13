void PlayerGeneric::adjustSettings()
{
	mp_uint32 bufferSize = mixer->getBufferSize();
	mp_uint32 sampleRate = mixer->getSampleRate();
	
	this->bufferSize = bufferSize;
	this->frequency = sampleRate;
	
	if (player)
	{
		player->setBufferSize(bufferSize);
		player->adjustFrequency(sampleRate);
	}
}