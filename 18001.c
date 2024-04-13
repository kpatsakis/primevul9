_XkbCreateIndicatorMap(DeviceIntPtr dev, Atom indicator,
                       int ledClass, int ledID,
                       XkbIndicatorMapPtr *map_return, int *led_return,
                       Bool dryRun)
{
    XkbSrvLedInfoPtr    sli;
    XkbIndicatorMapPtr  map;
    int                 led;

    sli = XkbFindSrvLedInfo(dev, ledClass, ledID, XkbXI_IndicatorsMask);
    if (!sli)
        return BadAlloc;

    map = _XkbFindNamedIndicatorMap(sli, indicator, &led);

    if (!map)
    {
        /* find first unused indicator maps and assign the name to it */
        for (led = 0, map = NULL; (led < XkbNumIndicators) && (map == NULL); led++) {
            if ((sli->names) && (sli->maps) && (sli->names[led] == None) &&
                    (!XkbIM_InUse(&sli->maps[led])))
            {
                map = &sli->maps[led];
                if (!dryRun)
                    sli->names[led] = indicator;
                break;
            }
        }
    }

    if (!map)
        return BadAlloc;

    *led_return = led;
    *map_return = map;
    return Success;
}