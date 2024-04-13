	void calculateMasterVolume()
	{
		if (lastPeakValue)
		{
			float v = 32768.0f*(1<<mixerShift) / (float)lastPeakValue;
			masterVolume = (mp_sint32)((float)masterVolume*v);
			if (masterVolume > 256)
				masterVolume = 256;
		}
	}