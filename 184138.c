GlobalClassExit(void)
{
	DEFiRet;

	/* first, release everything we used ourself */
	objRelease(net,      LM_NET_FILENAME);/* TODO: the dependency on net shall go away! -- rgerhards, 2008-03-07 */
	objRelease(prop,     CORE_COMPONENT);
	objRelease(conf,     CORE_COMPONENT);
	objRelease(ruleset,  CORE_COMPONENT);
	objRelease(rule,     CORE_COMPONENT);
	objRelease(expr,     CORE_COMPONENT);
	vmClassExit();					/* this is hack, currently core_modules do not get this automatically called */
	objRelease(datetime, CORE_COMPONENT);

	/* TODO: implement the rest of the deinit */
	/* dummy "classes */
	strExit();

#if 0
	CHKiRet(objGetObjInterface(&obj)); /* this provides the root pointer for all other queries */
	/* the following classes were intialized by objClassInit() */
	CHKiRet(objUse(errmsg,   CORE_COMPONENT));
	CHKiRet(objUse(module,   CORE_COMPONENT));
#endif
	rsrtExit(); /* *THIS* *MUST/SHOULD?* always be the first class initilizer being called (except debug)! */

	RETiRet;
}