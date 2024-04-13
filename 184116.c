InitGlobalClasses(void)
{
	DEFiRet;
	char *pErrObj; /* tells us which object failed if that happens (useful for troubleshooting!) */

	/* Intialize the runtime system */
	pErrObj = "rsyslog runtime"; /* set in case the runtime errors before setting an object */
	CHKiRet(rsrtInit(&pErrObj, &obj));
	CHKiRet(rsrtSetErrLogger(submitErrMsg)); /* set out error handler */

	/* Now tell the system which classes we need ourselfs */
	pErrObj = "glbl";
	CHKiRet(objUse(glbl,     CORE_COMPONENT));
	pErrObj = "errmsg";
	CHKiRet(objUse(errmsg,   CORE_COMPONENT));
	pErrObj = "module";
	CHKiRet(objUse(module,   CORE_COMPONENT));
	pErrObj = "datetime";
	CHKiRet(objUse(datetime, CORE_COMPONENT));
	pErrObj = "expr";
	CHKiRet(objUse(expr,     CORE_COMPONENT));
	pErrObj = "rule";
	CHKiRet(objUse(rule,     CORE_COMPONENT));
	pErrObj = "ruleset";
	CHKiRet(objUse(ruleset,  CORE_COMPONENT));
	pErrObj = "conf";
	CHKiRet(objUse(conf,     CORE_COMPONENT));
	pErrObj = "prop";
	CHKiRet(objUse(prop,     CORE_COMPONENT));

	/* intialize some dummy classes that are not part of the runtime */
	pErrObj = "action";
	CHKiRet(actionClassInit());
	pErrObj = "template";
	CHKiRet(templateInit());
	pErrObj = "parser";
	CHKiRet(parserClassInit());

	/* TODO: the dependency on net shall go away! -- rgerhards, 2008-03-07 */
	pErrObj = "net";
	CHKiRet(objUse(net, LM_NET_FILENAME));

finalize_it:
	if(iRet != RS_RET_OK) {
		/* we know we are inside the init sequence, so we can safely emit
		 * messages to stderr. -- rgerhards, 2008-04-02
		 */
		fprintf(stderr, "Error during class init for object '%s' - failing...\n", pErrObj);
	}

	RETiRet;
}