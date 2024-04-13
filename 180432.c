ospf_api_typename (int msgtype)
{
  struct nametab NameTab[] = {
    { MSG_REGISTER_OPAQUETYPE,   "Register opaque-type",   },
    { MSG_UNREGISTER_OPAQUETYPE, "Unregister opaque-type", },
    { MSG_REGISTER_EVENT,        "Register event",         },
    { MSG_SYNC_LSDB,             "Sync LSDB",              },
    { MSG_ORIGINATE_REQUEST,     "Originate request",      },
    { MSG_DELETE_REQUEST,        "Delete request",         },
    { MSG_REPLY,                 "Reply",                  },
    { MSG_READY_NOTIFY,          "Ready notify",           },
    { MSG_LSA_UPDATE_NOTIFY,     "LSA update notify",      },
    { MSG_LSA_DELETE_NOTIFY,     "LSA delete notify",      },
    { MSG_NEW_IF,                "New interface",          },
    { MSG_DEL_IF,                "Del interface",          },
    { MSG_ISM_CHANGE,            "ISM change",             },
    { MSG_NSM_CHANGE,            "NSM change",             },
  };

  int i, n = array_size(NameTab);
  const char *name = NULL;

  for (i = 0; i < n; i++)
    {
      if (NameTab[i].value == msgtype)
        {
          name = NameTab[i].name;
          break;
        }
    }

  return name ? name : "?";
}