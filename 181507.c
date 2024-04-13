bool Inflator::IsolatedFlush(bool hardFlush, bool blocking)
{
	if (!blocking)
		throw BlockingInputOnly("Inflator");

	if (hardFlush)
		ProcessInput(true);
	FlushOutput();

	return false;
}