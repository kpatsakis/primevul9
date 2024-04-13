xmlXPathFreeObjectEntry(void *obj, const xmlChar *name ATTRIBUTE_UNUSED) {
    xmlXPathFreeObject((xmlXPathObjectPtr) obj);
}