decodeSetting(const char* key, const char* value, VersionSetting* settings, IDATA numSettings)
{
	IDATA index = 0;
	
	for (index = 0; index < numSettings; index++) {
		VersionSetting *setting = &settings[index];
		if (!strcmp(value, setting->key)) {
			return setting->value;
		}
	}

	if (0 == strlen(value)) {
		fprintf(stderr, "Empty shape value was set!\n");
	} else if (('b' != *value) && ('B' != *value)) {
		fprintf(stderr, "Unrecognized '%s' value -> %s\n", key, value);
	} else {
		int levelValue = atoi((const char*)(value + 1));
		if (levelValue > 0) {
			for (index = numSettings - 1; index >= 0; index--) {
				VersionSetting *setting = &settings[index];
				if ('b' == *setting->key) {
					int keyNbr = atoi((const char*)(setting->key + 1));
					if (keyNbr > 0) {
						if (keyNbr <= levelValue) {
							return setting->value;
						}
					} else {
						continue;
					}
				}
			}
			fprintf(stderr, "There was no matching level found for '%s' value -> %s\n", key, value);
		} else {
			fprintf(stderr, "Unrecognized '%s' value -> %s\n", key, value);
		}
	}

#if defined(DEBUG)	
	printf("Valid choices are: ");
	for (index=0; index < numSettings; index++) {
		VersionSetting* setting = &settings[index];
		printf("%s", setting->key);
		if (index != numSettings-1)
				printf(", ");		
	}
	printf(".\n");
#endif
	return 0;
}