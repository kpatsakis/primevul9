testBackupAndRestoreLibpath(void)
{
	int failed = 0;
	int passed = 0;
	char *origLibpath = getenv("LIBPATH");

	J9LibpathBackup bkp = {NULL, 0};

	printf("testBackupAndRestoreLibpath:------------ BEGIN ------------------------\n");

	/*
	 * Typical tests
	 */

	/* Remove the path added by VM when restoring LIBPATH */
	printf("TESTCASE_1: Remove the path added by VM when restoring LIBPATH\n");
	setLibpath("compressedrefs:/usr/lib");
	backupLibpath(&bkp, strlen("/usr/lib"));
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "/usr/lib")) {
		printf("Test result: PASSED at </usr/lib>\n");
		passed++;
	} else {
		fprintf(stderr, "Test result: FAILED at </usr/lib>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Ignore multiple colons prefixing the backup path when restoring LIBPATH */
	printf("TESTCASE_2: Ignore colons prefixing the backup path when restoring LIBPATH\n");
	setLibpath("::abc");
	backupLibpath(&bkp, strlen("abc"));
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc")) {
		printf("Test result: PASSED at <abc>\n");
		passed++;
	} else {
		fprintf(stderr, "Test result: FAILED at <abc>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/*
	 * Test fullpath searching
	 */

	/* Set up LIBPATH again with exactly the same path */
	printf("TESTCASE_3: Set up libpath again with exactly the same path\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("x:y");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "")) {
		printf("testcase_3: PASSED at <>\n");
		passed++;
	} else {
		fprintf(stderr, "testcase_3: FAILED at <>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path is followed by a single colon */
	printf("TESTCASE_4: The backup path is followed by a single colon\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("x:y:");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "")) {
		printf("testcase_4: PASSED at <>\n");
		passed++;
	} else {
		fprintf(stderr, "testcase_4: FAILED at <>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path is followed by multiple colons */
	printf("TESTCASE_5: The backup path is followed by multiple colons\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("x:y:::");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "")) {
		printf("TESTCASE_5: PASSED at <>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_5: FAILED at <>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path is followed by multiple paths */
	printf("TESTCASE_6: The backup path is followed by multiple paths\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("x:y:def:ghi");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "def:ghi")) {
		printf("TESTCASE_6: PASSED at <def:ghi>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_6: FAILED at <def:ghi>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the start except for a single colon */
	printf("TESTCASE_7: The backup path stays at the start except for a single colon\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath(":x:y:def:ghi");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "def:ghi")) {
		printf("TESTCASE_7: PASSED at <def:ghi>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_7: FAILED at <def:ghi>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the start except for multiple colons */
	printf("TESTCASE_8: The backup path stays at the start except for multiple colons\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath(":::x:y:def:ghi");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "def:ghi")) {
		printf("TESTCASE_8: PASSED at <def:ghi>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_8: FAILED at <def:ghi>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Partial match at the start */
	printf("TESTCASE_9: Partial match at the start\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("::x:ydef:ghi");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "::x:ydef:ghi")) {
		printf("TESTCASE_9: PASSED at <::x:ydef:ghi>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_9: FAILED at <::x:ydef:ghi>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Partial match at the end */
	printf("TESTCASE_10: Partial match at the end\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("::abc:defx:y::");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "::abc:defx:y::")) {
		printf("TESTCASE_10: PASSED at <::abc:defx:y::>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_10: FAILED at <::abc:defx:y::>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Partial match in the middle */
	printf("TESTCASE_11: Partial match in the middle \n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("::abc:def::x:yghi:jkl");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "::abc:def::x:yghi:jkl")) {
		printf("TESTCASE_11: PASSED at <::abc:def::x:yghi:jkl>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_11: FAILED at <::abc:def::x:yghi:jkl>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the end with a single colon ahead of it */
	printf("TESTCASE_12: The backup path stays at the end with a single colon ahead of it\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath(":x:y");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "")) {
		printf("TESTCASE_12: PASSED at <>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_12: FAILED at <>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the end with multiple colons ahead of it */
	printf("TESTCASE_13: The backup path stays at the end with multiple colons ahead of it\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("::x:y");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "")) {
		printf("TESTCASE_13: PASSED at <>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_13: FAILED at <>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the end with multiple paths ahead of it */
	printf("TESTCASE_14: The backup path stays at the end with multiple paths ahead of it\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("abc:def:x:y");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc:def")) {
		printf("TESTCASE_14: PASSED at <abc:def>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_14: FAILED at <abc:def>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the end with multiple empty paths ahead of it */
	printf("TESTCASE_15: The backup path stays at the end with multiple empty paths ahead of it\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("abc:::def:x:y");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc:::def")) {
		printf("TESTCASE_15: PASSED at <abc:::def>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_15: FAILED at <abc:::def>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* An empty path plus a single colon at the start stays ahead of the backup path */
	printf("TESTCASE_16: An empty path plus a single colon at the start stays ahead of the backup path\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath(":abc::def:x:y");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), ":abc::def")) {
		printf("TESTCASE_16: PASSED at <:abc::def>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_16: FAILED at <:abc::def>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Multiple empty paths plus a single colon at the start stays ahead of the backup path */
	printf("TESTCASE_17: Multiple empty paths plus a single colon at the start stays ahead of the backup patht\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath(":abc::def::x:y");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), ":abc::def")) {
		printf("TESTCASE_17: PASSED at <:abc::def>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_17: FAILED at <:abc::def>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the end except for a single colon */
	printf("TESTCASE_18: The backup path stays at the end except for a single colon\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("abc:def:x:y:");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc:def")) {
		printf("TESTCASE_18: PASSED at <abc:def>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_18: FAILED at <abc:def>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path stays at the end except for an empty path */
	printf("TESTCASE_19: The backup path stays at the end except for an empty path\n");
	setLibpath("x:y");
	backupLibpath(&bkp, 0);
	setLibpath("abc:def:x:y::");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc:def")) {
		printf("TESTCASE_19: PASSED at <abc:def>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_19: FAILED at <abc:def>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path comes with a trailing colon */
	printf("TESTCASE_20: The backup path comes with a trailing colon\n");
	setLibpath("x:y:");
	backupLibpath(&bkp, 0);
	setLibpath("abc:x:y:ghi");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc:ghi")) {
		printf("TESTCASE_20: PASSED at <abc:ghi>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_20: FAILED at <abc:ghi>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path comes with a leading colon */
	printf("TESTCASE_21: The backup path comes with a leading colon\n");
	setLibpath(":x:y");
	backupLibpath(&bkp, 0);
	setLibpath("::abc:def::x:y:ghi");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "::abc:def:ghi")) {
		printf("TESTCASE_21: PASSED at <::abc:def:ghi>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_21: FAILED at <::abc:def:ghi>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/*
	 * Test path deletion
	 */

	/* Delete from the start when restoring LIBPATH */
	printf("TESTCASE_22: Delete from the start when restoring LIBPATH\n");
	setLibpath("abc::def:xyz");
	backupLibpath(&bkp, strlen("xyz"));
	setLibpath("abc::def:xyz:morestuff");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "xyz:morestuff")) {
		printf("TESTCASE_22: PASSED at <xyz:morestuff>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_22: FAILED at <xyz:morestuff>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Delete from the middle when restoring LIBPATH */
	printf("TESTCASE_23: Delete from the middle when restoring LIBPATH\n");
	setLibpath("abc::def:xyz");
	backupLibpath(&bkp, strlen("xyz"));
	setLibpath(":stuff:abc::def:xyz:morestuff");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), ":stuff:xyz:morestuff")) {
		printf("TESTCASE_23: PASSED at <:stuff:xyz:morestuff>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_23: FAILED at <:stuff:xyz:morestuff>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Delete from the end when restoring LIBPATH */
	printf("TESTCASE_24: Delete from the end when restoring LIBPATH\n");
	setLibpath("abc::def:xyz");
	backupLibpath(&bkp, strlen("xyz"));
	setLibpath(":stuff:morestuff:abc::def:xyz");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), ":stuff:morestuff:xyz")) {
		printf("TESTCASE_24: PASSED at <:stuff:morestuff:xyz>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_24: FAILED at <:stuff:morestuff:xyz>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Ignore colons in the prefix when restoring LIBPATH */
	printf("TESTCASE_25: Ignore colons in the prefix when restoring LIBPATH\n");
	setLibpath("abc::def::xyz");
	backupLibpath(&bkp, strlen("xyz"));
	setLibpath("stuff:abc::def::xyz:morestuff");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "stuff:xyz:morestuff")) {
		printf("TESTCASE_25: PASSED at <stuff:xyz:morestuff>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_25: FAILED at <stuff:xyz:morestuff>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Skip colons from both start and end when restoring LIBPATH */
	printf("TESTCASE_26: Skip colons from both start and end when restoring LIBPATH\n");
	setLibpath("abc::def:");
	backupLibpath(&bkp, 0);
	setLibpath("::abc::def::");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "")) {
		printf("TESTCASE_26: PASSED at <>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_26: FAILED at <>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Insert colon from the end when restoring LIBPATH */
	printf("TESTCASE_27: Insert colon from the end when restoring LIBPATH\n");
	setLibpath(":abc::def:xyz");
	backupLibpath(&bkp, strlen("xyz"));
	setLibpath("stuff:abc::def:xyz:morestuff");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "stuff:xyz:morestuff")) {
		printf("TESTCASE_27: PASSED at <stuff:xyz:morestuff>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_27: FAILED at <stuff:xyz:morestuff>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Empty backup path before restoring LIBPATH */
	printf("TESTCASE_28: Empty backup path before restoring LIBPATH\n");
	setLibpath("");
	backupLibpath(&bkp, 0);
	setLibpath("stuff:abc::def:xyz:morestuff");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "stuff:abc::def:xyz:morestuff")) {
		printf("TESTCASE_28: PASSED at <stuff:abc::def:xyz:morestuff>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_28: FAILED at <stuff:abc::def:xyz:morestuff>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Keep backup path and empty prefix when restoring LIBPATH */
	printf("TESTCASE_29: Keep backup path and empty prefix when restoring LIBPATH\n");
	setLibpath("abc:def");
	backupLibpath(&bkp, strlen("abc:def"));
	setLibpath("stuff:abc:def:xyz:morestuff");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "stuff:abc:def:xyz:morestuff")) {
		printf("TESTCASE_29: PASSED at <stuff:abc:def:xyz:morestuff>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_29: FAILED at <stuff:abc:def:xyz:morestuff>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The length of current path is greater than the backup path when restoring LIBPATH */
	printf("TESTCASE_30: The length of current path is greater than the backup path when restoring LIBPATH\n");
	setLibpath("abc:def:ghi");
	backupLibpath(&bkp, 0);
	setLibpath("abc:def");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc:def")) {
		printf("TESTCASE_30: PASSED at <abc:def>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_30: FAILED at <abc:def>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* The backup path is prefixed by mulitiple colons */
	printf("TESTCASE_31: The backup path is prefixed by mulitiple colons\n");
	setLibpath(":::abc");
	backupLibpath(&bkp, strlen("abc"));
	setLibpath("abc");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc")) {
		printf("TESTCASE_31: PASSED at <abc>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_31: FAILED at <abc>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Only VM prefix exists in the backup path when restoring LIBPATH */
	printf("TESTCASE_32: Only VM prefix exists in the backup path when restoring LIBPATH\n");
	setLibpath(":::abc");
	backupLibpath(&bkp, 0);
	setLibpath("abc");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "abc")) {
		printf("TESTCASE_32: PASSED at <abc>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_32: FAILED at <abc>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Colons prefixing the backup LIBPATH is more than that of the current LIBPATH */
	printf("TESTCASE_33: Colons prefixing the backup LIBPATH is more than that of the current LIBPATH\n");
	setLibpath(":::abc:xyz");
	backupLibpath(&bkp, strlen("abc:xyz"));
	setLibpath("def::abc:xyz");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "def::abc:xyz")) {
		printf("TESTCASE_33: PASSED at <def::abc:xyz>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_33: FAILED at <def::abc:xyz>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Colons prefixing the backup LIBPATH is less than that of the current LIBPATH (1) */
	printf("TESTCASE_34: Colons prefixing the backup LIBPATH is less than that of the current LIBPATH (1)\n");
	setLibpath(":::abc:xyz");
	backupLibpath(&bkp, strlen("abc:xyz"));
	setLibpath("def:::abc:xyz");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "def:abc:xyz")) {
		printf("TESTCASE_34: PASSED at <def:abc:xyz>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_34: FAILED at <def:abc:xyz>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	/* Colons prefixing the backup LIBPATH is less than that of the current LIBPATH (2) */
	printf("TESTCASE_35: Colons prefixing the backup LIBPATH is less than that of the current LIBPATH (2)\n");
	setLibpath(":::abc:xyz");
	backupLibpath(&bkp, strlen("abc:xyz"));
	setLibpath("def::::abc:xyz");
	restoreLibpath(&bkp);
	if (0 == strcmp(getenv("LIBPATH"), "def:abc:xyz")) {
		printf("TESTCASE_35: PASSED at <def:abc:xyz>\n");
		passed++;
	} else {
		fprintf(stderr, "TESTCASE_35: FAILED at <def:abc:xyz>: LIBPATH = <%s>\n", getenv("LIBPATH"));
		failed++;
	}

	printf("---------- TEST RESULTS ----------\n");
	printf("Number of PASSED tests: %d\n", passed);
	printf("Number of FAILED tests: %d\n", failed);

	if (0 == failed) {
		printf("testBackupAndRestoreLibpath:---- TEST_PASSED ---------\n");
	} else {
		printf("testBackupAndRestoreLibpath:---- TEST_FAILED ---------\n");
	}

	setLibpath(origLibpath);
}