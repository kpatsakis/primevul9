xsltAddChild(xmlNodePtr parent, xmlNodePtr cur) {
   xmlNodePtr ret;

   if ((cur == NULL) || (parent == NULL))
       return(NULL);
   if (parent == NULL) {
       xmlFreeNode(cur);
       return(NULL);
   }
   ret = xmlAddChild(parent, cur);

   return(ret);
}