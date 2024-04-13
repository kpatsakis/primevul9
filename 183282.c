int psr_print(void)
{
	uint8_t array[256];
	uint16_t pskey, length;
	char *str, val[7];
	int i;

	while (1) {
		if (psr_get(&pskey, array, &length) < 0)
			break;

		str = csr_pskeytoval(pskey);
		if (!strcasecmp(str, "UNKNOWN")) {
			sprintf(val, "0x%04x", pskey);
			str = NULL;
		}

		printf("// %s%s\n&%04x =", str ? "PSKEY_" : "",
						str ? str : val, pskey);
		for (i = 0; i < length / 2; i++)
			printf(" %02x%02x", array[i * 2 + 1], array[i * 2]);
		printf("\n");
	}

	return 0;
}