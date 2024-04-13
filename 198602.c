static int verify_pin(int pin)
{
	char prompt[50];
	int r, tries_left = -1;

	if (pincode == NULL) {
		sprintf(prompt, "Please enter CHV%d: ", pin);
		pincode = (u8 *) getpin(prompt);
		if (pincode == NULL || strlen((char *) pincode) == 0)
			return -1;
	}
	if (pin != 1 && pin != 2)
		return -3;
	r = sc_verify(card, SC_AC_CHV, pin, pincode, 8, &tries_left);
	if (r) {
		memset(pincode, 0, 8);
		free(pincode);
		pincode = NULL;
		fprintf(stderr, "PIN code verification failed: %s\n", sc_strerror(r));
		return -1;
	}
	return 0;
}