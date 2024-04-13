	FilterTester(const byte *validOutput, size_t outputLen)
		: validOutput(validOutput), outputLen(outputLen), counter(0), fail(false) {}