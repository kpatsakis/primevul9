_XkbFindNamedIndicatorMap(XkbSrvLedInfoPtr sli, Atom indicator,
                          int *led_return)
{
    XkbIndicatorMapPtr  map;
    int                 led;

    /* search for the right indicator */
    map = NULL;
    if (sli->names && sli->maps) {
	for (led = 0; (led < XkbNumIndicators) && (map == NULL); led++) {
	    if (sli->names[led] == indicator) {
		map= &sli->maps[led];
		break;
	    }
	}
    }

    *led_return = led;
    return map;
}