static int setcos_pin_index_44(int *pins, int len, int pin)
{
	int i;
	for (i = 0; i < len; i++) {
		if (pins[i] == pin)
			return i;
		if (pins[i] == -1) {
			pins[i] = pin;
			return i;
		}
	}
	assert(i != len); /* Too much PINs, shouldn't happen */
	return 0;
}