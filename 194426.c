exsltDynRegister (void) {
    xsltRegisterExtModuleFunction ((const xmlChar *) "evaluate",
				   EXSLT_DYNAMIC_NAMESPACE,
				   exsltDynEvaluateFunction);
  xsltRegisterExtModuleFunction ((const xmlChar *) "map",
				   EXSLT_DYNAMIC_NAMESPACE,
				   exsltDynMapFunction);

}