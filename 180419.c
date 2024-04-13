ospf_api_errname (int errcode)
{
  struct nametab NameTab[] = {
    { OSPF_API_OK,                      "OK",                         },
    { OSPF_API_NOSUCHINTERFACE,         "No such interface",          },
    { OSPF_API_NOSUCHAREA,              "No such area",               },
    { OSPF_API_NOSUCHLSA,               "No such LSA",                },
    { OSPF_API_ILLEGALLSATYPE,          "Illegal LSA type",           },
    { OSPF_API_OPAQUETYPEINUSE,         "Opaque type in use",         },
    { OSPF_API_OPAQUETYPENOTREGISTERED, "Opaque type not registered", },
    { OSPF_API_NOTREADY,                "Not ready",                  },
    { OSPF_API_NOMEMORY,                "No memory",                  },
    { OSPF_API_ERROR,                   "Other error",                },
    { OSPF_API_UNDEF,                   "Undefined",                  },
  };

  int i, n = array_size(NameTab);
  const char *name = NULL;

  for (i = 0; i < n; i++)
    {
      if (NameTab[i].value == errcode)
        {
          name = NameTab[i].name;
          break;
        }
    }

  return name ? name : "?";
}