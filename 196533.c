CameraMetaDataLR::CameraMetaDataLR(char *data, int sz) : CameraMetaData() {
  ctxt = xmlNewParserCtxt();
  if (ctxt == NULL) {
    ThrowCME("CameraMetaData:Could not initialize context.");
  }
  
  xmlResetLastError();
  doc = xmlCtxtReadMemory(ctxt, data,sz, "", NULL, XML_PARSE_DTDVALID);
  
  if (doc == NULL) {
    ThrowCME("CameraMetaData: XML Document could not be parsed successfully. Error was: %s", ctxt->lastError.message);
  }
  
  if (ctxt->valid == 0) {
    if (ctxt->lastError.code == 0x5e) {
      // printf("CameraMetaData: Unable to locate DTD, attempting to ignore.");
    } else {
      ThrowCME("CameraMetaData: XML file does not validate. DTD Error was: %s", ctxt->lastError.message);
    }
  }
  
  xmlNodePtr cur;
  cur = xmlDocGetRootElement(doc);
  if (xmlStrcmp(cur->name, (const xmlChar *) "Cameras")) {
    ThrowCME("CameraMetaData: XML document of the wrong type, root node is not cameras.");
    return;
  }
  
  cur = cur->xmlChildrenNode;
  while (cur != NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"Camera"))) {
      Camera *camera = new Camera(doc, cur);
      addCamera(camera);
      
      // Create cameras for aliases.
      for (uint32 i = 0; i < camera->aliases.size(); i++) {
        addCamera(new Camera(camera, i));
      }
    }
    cur = cur->next;
  }
  if (doc)
    xmlFreeDoc(doc);
  doc = 0;
  if (ctxt)
    xmlFreeParserCtxt(ctxt);
  ctxt = 0;
}