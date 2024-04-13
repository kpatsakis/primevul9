	virtual void mix(mp_sint32* buffer, mp_uint32 bufferSize)
	{
		const mp_sint32* buffer32 = buffer;
		
		for (mp_uint32 i = 0; i < bufferSize*MP_NUMCHANNELS; i++)
		{
			mp_sint32 b = *buffer32++;
			
			if (abs(b) > lastPeakValue)
				lastPeakValue = abs(b);					
		}
	}