ProcXkbDispatch (ClientPtr client)
{
    REQUEST(xReq);
    switch (stuff->data)
    {
    case X_kbUseExtension:
	return ProcXkbUseExtension(client);
    case X_kbSelectEvents:
	return ProcXkbSelectEvents(client);
    case X_kbBell:
	return ProcXkbBell(client);
    case X_kbGetState:
	return ProcXkbGetState(client);
    case X_kbLatchLockState:
	return ProcXkbLatchLockState(client);
    case X_kbGetControls:
	return ProcXkbGetControls(client);
    case X_kbSetControls:
	return ProcXkbSetControls(client);
    case X_kbGetMap:
	return ProcXkbGetMap(client);
    case X_kbSetMap:
	return ProcXkbSetMap(client);
    case X_kbGetCompatMap:
	return ProcXkbGetCompatMap(client);
    case X_kbSetCompatMap:
	return ProcXkbSetCompatMap(client);
    case X_kbGetIndicatorState:
	return ProcXkbGetIndicatorState(client);
    case X_kbGetIndicatorMap:
	return ProcXkbGetIndicatorMap(client);
    case X_kbSetIndicatorMap:
	return ProcXkbSetIndicatorMap(client);
    case X_kbGetNamedIndicator:
	return ProcXkbGetNamedIndicator(client);
    case X_kbSetNamedIndicator:
	return ProcXkbSetNamedIndicator(client);
    case X_kbGetNames:
	return ProcXkbGetNames(client);
    case X_kbSetNames:
	return ProcXkbSetNames(client);
    case X_kbGetGeometry:
	return ProcXkbGetGeometry(client);
    case X_kbSetGeometry:
	return ProcXkbSetGeometry(client);
    case X_kbPerClientFlags:
	return ProcXkbPerClientFlags(client);
    case X_kbListComponents:
	return ProcXkbListComponents(client);
    case X_kbGetKbdByName:
	return ProcXkbGetKbdByName(client);
    case X_kbGetDeviceInfo:
	return ProcXkbGetDeviceInfo(client);
    case X_kbSetDeviceInfo:
	return ProcXkbSetDeviceInfo(client);
    case X_kbSetDebuggingFlags:
	return ProcXkbSetDebuggingFlags(client);
    default:
	return BadRequest;
    }
}