mp_sint32 PlayerGeneric::setPowerOfTwoCompensationFlag(bool b)
{
	if (mixer && compensateBufferFlag != b)
	{
		compensateBufferFlag = b;
		setBufferSize(bufferSize);
	}

	return MP_OK;
}