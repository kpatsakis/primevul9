static void rcc_reset_callname(REgg *egg) {
	R_FREE (egg->lang.callname);
	egg->lang.nargs = 0;
}