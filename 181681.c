int main(void)
{
	int i;

	for (i = 0; tests[i].hash; i++) {
		char *hash = crypt(tests[i].pw, tests[i].hash);
		if (!hash && strlen(tests[i].hash) < 13)
			continue; /* expected failure */
		if (!strcmp(hash, tests[i].hash))
			continue; /* expected success */
		puts("FAILED");
		return 1;
	}

	puts("PASSED");

	return 0;
}