const char *XMLRPC_SetValueID_Case(XMLRPC_VALUE value, const char* id, int len, XMLRPC_CASE id_case) {
   const char* pRetval = NULL;
   if(value) {
      if(id) {
         simplestring_clear(&value->id);
         (len > 0) ? simplestring_addn(&value->id, id, len) :
                     simplestring_add(&value->id, id);

         /* upper or lower case string in place if required. could be a separate func. */
         if(id_case == xmlrpc_case_lower || id_case == xmlrpc_case_upper) {
            int i;
            for(i = 0; i < value->id.len; i++) {
					value->id.str[i] =
					(id_case ==
					 xmlrpc_case_lower) ? tolower (value->id.
															 str[i]) : toupper (value->
																					  id.
																					  str[i]);
            }
         }

         pRetval = value->id.str;

#ifdef XMLRPC_DEBUG_REFCOUNT
         printf("set value id: %s\n", pRetval);
#endif 
      }
   }

   return pRetval;
}