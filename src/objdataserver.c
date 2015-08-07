/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework_base.c"
#include "cstring.c"
#include "event.c"
#include "objpersistent.c"
#include "objcontainer.c"
#include "syncobject.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

ARRAY:typedef<CObjPersistent *>;
class CObjServer;

ENUM:typedef<EEventObjPersistent> {
   {AllUpdate},
   {AllDelete},
   {ObjectNew},
   {ObjectDelete},
   {AttributeSet},
   {AttributeUpdateEnd},
   {SelectionUpdate},
};

class CEventObjPersistent : CEvent {
 private:
   EEventObjPersistent type;

   CString path;
   CString value;

   void new(void);
   void delete(void);
 public:
   void CEventObjPersistent(EEventObjPersistent type, const char *path, const char *value);
};

class CObjClient : CObject {
 private:
   CObjServer *server;
   CObject *host;
   ARRAY<CObjPersistent *> object_root; /*>>>convert to single root only*/
   CThread *thread;
   bool disable;
   //>>>CSelection *selection;

   void new(void);
   void delete(void);
 public:
   void CObjClient(CObjServer *server, CObject *host);

   void disable(bool disable);
   void object_root_add(CObjPersistent *object_root);
   static inline bool object_subscribed(CObject *object);

   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_selection_update(CObjPersistent *object, bool changing);
};

static inline bool CObjClient::object_subscribed(CObject *object) {
   int i;

   if (this->disable) {
      return FALSE;
   }

   while (object) {
      for (i = 0; i < ARRAY(&this->object_root).count(); i++) {
         if (object == CObject(ARRAY(&this->object_root).data()[i])) {
            return TRUE;
         }
      }
      object = CObject(object).parent();
   }
   return FALSE;
};

ARRAY:typedef<CObjClient *>;

typedef struct {
   CObjPersistent *object;
   CThread *thread;
} TObjectThreadOwner;

typedef struct {
   enum {attribute_update, child_add, child_remove} type;
   CThread *dest_thread;
   CObjPersistent *object;
   TAttribute *attribute;
   int index;
} TServerEvent;

ARRAY:typedef<TObjectThreadOwner *>;
ARRAY:typedef<CEventObjPersistent>;

class CObjServer : CObjPersistent {
 private:
   bool disable;

   CObjPersistent *server_root;
   ARRAY<CObjClient *> client;
   ARRAY<TObjectThreadOwner *> thread_owner;
   ARRAY<CEventObjPersistent> obj_event;
//   LIST<TServerEvent> undo_foward;
//   LIST<TServerEvent> undo_backward;

   CSelection *selection;

   void new(void);
   void delete(void);

   void attribute_update(CObjPersistent *object,
                         ARRAY<const TAttribute *> *attribute);
   void child_add(CObjPersistent *object, CObjPersistent *child);
   void child_remove(CObjPersistent *object, CObjPersistent *child);
   void selection_update(CObjPersistent *object, bool changing);
 public:
   void CObjServer(CObjPersistent *object);
   void disable(bool disable);

//   CObjPersistent *object_copy(CObjPersistent *object, int index);

//   CString name;
//   CSelection *selection;
   void server_root_set(CObjPersistent *object);
   int object_root_count(CObjPersistent *object);
   CThread *thread_owner_find(CObjPersistent *object);
   void client_add(CObjClient *client);
   void client_remove(CObjClient *client);

   static inline void selection_set(CSelection *selection);
   static inline CSelection *selection_get(void);

   void root_selection_update(void);
   void root_selection_update_end(void);

   void undo(void);
   void redo(void);
};

static inline void CObjServer::selection_set(CSelection *selection) {
   this->selection = selection;
}/*CObjServer::selection_set*/

static inline CSelection *CObjServer::selection_get(void) {
   return this->selection;
}/*CObjServer::selection_get*/

ARRAY:typedef<CObjServer *>;

class CFrameworkServer : CObjPersistent {
 private:
   ARRAY<CObjServer *> obj_server;

   void new(void);
   void delete(void);
 public:
   void CFrameworkServer(void);

   void obj_server_add(CObjServer *obj_server);
   void obj_server_remove(CObjServer *obj_server);
   CObjServer *object_obj_server(CObjPersistent *object);
   CObjClient *object_obj_client(CObjPersistent *object);
};

extern CFrameworkServer framework_server;

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CFrameworkServer::new(void) {
   ARRAY(&this->obj_server).new();
}/*CFrameworkServer::new*/

void CFrameworkServer::delete(void) {
   ARRAY(&this->obj_server).delete();
}/*CFrameworkServer::delete*/

void CFrameworkServer::obj_server_add(CObjServer *obj_server) {
   ARRAY(&this->obj_server).item_add(obj_server);
}/*CFrameworkServer::obj_server_add*/

void CFrameworkServer::obj_server_remove(CObjServer *obj_server) {
   ARRAY(&this->obj_server).item_remove(obj_server);
}/*CFrameworkServer::obj_server_remove*/

CObjServer *CFrameworkServer::object_obj_server(CObjPersistent *object) {
   int i;
   CObjServer *server;
   CObjPersistent *root;

   for (i = ARRAY(&this->obj_server).count() - 1; i >=0; i--) {
      server = ARRAY(&this->obj_server).data()[i];
      root = server->server_root;

      if ((root == object) || CObject(root).is_child(CObject(object))) {
         return server;
      }
   }

   return NULL;
}/*CFrameworkServer::object_obj_server*/

CObjClient *CFrameworkServer::object_obj_client(CObjPersistent *object) {
   int i, j;
   CObjServer *server;
   CObjClient *client;

   for (i = 0; i < ARRAY(&this->obj_server).count(); i++) {
      server = ARRAY(&this->obj_server).data()[i];
      for (j = 0; j < ARRAY(&server->client).count(); j++) {
         client = ARRAY(&server->client).data()[j];

         if (CObject(client->host).is_child(CObject(object))) {
            return client;
         }
      }
   }

   return NULL;
}/*CFrameworkServer::object_obj_client*/

OBJECT<CFrameworkServer, framework_server>;

void CXPath::root_set_default(CObjPersistent *parent) {
   CObjClient *client = CFrameworkServer(&framework_server).object_obj_client(parent);
   CObjServer *server = NULL;
   TObjectPtr initial;

   if (client) {
      server = client->server;
   }
   if (server) {
      CXPath(this).root_set(server->server_root);
      CLEAR(&initial);
      initial.object = ARRAY(&client->object_root).data()[0];
      initial.attr.element = -1;
      initial.attr.index   = -1;
      CXPath(this).initial_set(&initial);
   }
}/*CXPath::root_set_default*/

bool ATTRIBUTE:convert<CXPath>(CObjPersistent *object,
                               const TAttributeType *dest_type, const TAttributeType *src_type,
                               int src_index, int dest_index,
                               void *dest, const void *src) {
   CString *string;
   CXPath *x_path;
   CObjClient *client = CFrameworkServer(&framework_server).object_obj_client(object);
   CObjServer *server = NULL;
   TObjectPtr initial;

   if (client) {
      server = client->server;
   }

   if (dest_type == &ATTRIBUTE:type<CXPath> && src_type == NULL) {
      new(dest).CXPath(NULL, NULL);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CXPath> && src_type == &ATTRIBUTE:type<CXPath>) {
      x_path = (CXPath *)src;

      CXPath(dest)->root = CXPath(x_path)->root;
      CXPath(dest)->initial = CXPath(x_path)->initial;
//      CString(&CXPath(dest)->path).set(CString(&CXPath(x_path)->path).string());
      CXPath(dest).path_set(CString(&CXPath(x_path)->path).string());

      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CXPath> ) {
      delete(src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<CXPath>) {
      string = (CString *)dest;
      x_path = (CXPath *)src;
      if (server) {
         CXPath(x_path).path_get(string);
      }
      else {
         CString(string).set("(no server)");
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CXPath> && src_type == &ATTRIBUTE:type<CString>) {
      x_path = (CXPath *)dest;
      string = (CString *)src;
      if (server) {
         CXPath(x_path).root_set(server->server_root);
      }

      /*>>>for now*/
      if (client) {
         CLEAR(&initial);
         initial.object = ARRAY(&client->object_root).data()[0];
         CXPath(x_path).initial_set(&initial);
      }

      CXPath(x_path).path_set(CString(string).string());
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<TObjectPtr>(CObjPersistent *object,
                                   const TAttributeType *dest_type, const TAttributeType *src_type,
                                   int dest_index, int src_index,
                                   void *dest, const void *src) {
   CXPath path;
   TObjectPtr *objptr;
   CString *string;
   CObjClient *client = CFrameworkServer(&framework_server).object_obj_client(object);
   CObjServer *server = NULL;
//   CObjServer *server = CFrameworkServer(&framework_server).object_obj_server(object);

   if (client) {
      server = client->server;
   }

   if (dest_type == &ATTRIBUTE:type<TObjectPtr> && src_type == NULL) {
      memset(dest, 0, sizeof(TObjectPtr));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TObjectPtr> && src_type == &ATTRIBUTE:type<TObjectPtr>) {
      *((TObjectPtr *)dest) = *((TObjectPtr *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TObjectPtr> ) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> &&
      src_type == &ATTRIBUTE:type<TObjectPtr>) {
      objptr = (TObjectPtr *)src;
      if (objptr->object) {
         new(&path).CXPath(NULL, server->server_root);
         CXPath(&path).set(objptr->object, objptr->attr.attribute, -1);
         CXPath(&path).path_get(dest);
         delete(&path);
      }
      else {
         CString(dest).set("none");
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TObjectPtr> &&
       src_type == &ATTRIBUTE:type<CString>) {
      string = (CString *)src;

      new(&path).CXPath(NULL, server->server_root);
      objptr = (TObjectPtr *)dest;

      CXPath(&path).initial_set(dest);
      CXPath(&path).path_set(CString(string).string());
      *objptr = path.object;
      delete(&path);
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<TAttributePtr>(CObjPersistent *object,
                                      const TAttributeType *dest_type, const TAttributeType *src_type,
                                      int dest_index, int src_index,
                                      void *dest, const void *src) {
   TAttributePtr *attrptr;
   CString *string;
   const char *element_ptr, *index_ptr;

   if (dest_type == &ATTRIBUTE:type<TAttributePtr> && src_type == NULL) {
      memset(dest, 0, sizeof(TAttributePtr));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TAttributePtr> && src_type == &ATTRIBUTE:type<TAttributePtr>) {
      memcpy(dest, src, sizeof(TAttributePtr));
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TAttributePtr>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> &&
      src_type == &ATTRIBUTE:type<TAttributePtr>) {
      attrptr = (TAttributePtr *)src;
      if (attrptr->attribute) {
         CString(dest).set(attrptr->attribute->name);
         if (attrptr->element != -1) {
            CString(dest).printf_append("[%d]", attrptr->element);
         }
         if (attrptr->index != -1) {
            CString(dest).printf_append(".%d", attrptr->index);
         }
      }
      else {
         CString(dest).set("none");
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TAttributePtr> &&
       src_type == &ATTRIBUTE:type<CString>) {
      string = (CString *)src;
      attrptr = (TAttributePtr *)dest;

      attrptr->attribute = NULL;
      attrptr->element = -1;
      attrptr->index = -1;
      if (!CObject(object).parent()) {
         return TRUE;
      }

      index_ptr = strchr(CString(string).string(), '.');
      if (index_ptr) {
         attrptr->index = atoi(index_ptr + 1);
         CString(string).setn(CString(string).string(), index_ptr - CString(string).string());
      }
      element_ptr = strchr(CString(string).string(), '[');
      if (element_ptr) {
         attrptr->element = atoi(element_ptr + 1);
         CString(string).setn(CString(string).string(), element_ptr - CString(string).string());
      }
      attrptr->attribute = CObjPersistent(CObject(object).parent()).attribute_find(CString(string).string());

      return TRUE;
   }

   return FALSE;
}

void CEventObjPersistent::new(void) {
   new(&this->path).CString(NULL);
   new(&this->value).CString(NULL);
}/*CEventObjPersistent::new*/

void CEventObjPersistent::CEventObjPersistent(EEventObjPersistent type, const char *path, const char *value) {
   this->type = type;
   CString(&this->path).set(path);
   CString(&this->value).set(value);
}/*CEventObjPersistent::CEventObjPersistent*/

void CEventObjPersistent::delete(void) {
   delete(&this->value);
   delete(&this->path);
}/*CEventObjPersistent::delete*/

//OBJECT<CObjServer, obj_server>;

void CObjClient::new(void) {
   ARRAY(&this->object_root).new();
}/*CObjClient::new*/

void CObjClient::CObjClient(CObjServer *server, CObject *host) {
   if (!server) {
      ASSERT("no server\n");
   }

   this->server = server;
   this->host = host;
   this->thread = CFramework(&framework).thread_this();

   CObjServer(server).client_add(this);

   /* testing kludge */
   CObjClient(this).notify_all_update();
}/*CObjClient::CObjClient*/

void CObjClient::delete(void) {
   ARRAY(&this->object_root).delete();

   CObjServer(this->server).client_remove(this);
}/*CObjClient::delete*/

void CObjClient::disable(bool disable) {
   this->disable = disable;
}/*CObjClient::disable*/

void CObjClient::object_root_add(CObjPersistent *object_root) {
   if (ARRAY(&this->object_root).count() != 0) {
      WARN("CObjClient::object_root_add - single root only supported");
   }

   ARRAY(&this->object_root).item_add(object_root);
}/*CObjClient::object_root_add*/

void CObjClient::notify_all_update(void) {}
void CObjClient::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {}
void CObjClient::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {}
void CObjClient::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing) {}
void CObjClient::notify_selection_update(CObjPersistent *object, bool changing) {}

void CObjServer::new(void) {
   ARRAY(&this->client).new();
   ARRAY(&this->thread_owner).new();

   CFrameworkServer(&framework_server).obj_server_add(this);
}/*CObjServer::new()*/

void CObjServer::CObjServer(CObjPersistent *object) {
   if (object) {
      CObjServer(this).server_root_set(object);
   }
}/*CObjServer::CObjServer*/

void CObjServer::delete(void) {
   ARRAY(&this->thread_owner).delete();
   ARRAY(&this->client).delete();

   CFrameworkServer(&framework_server).obj_server_remove(this);
}/*CObjServer::delete*/

void CObjServer::disable(bool disable) {
   this->disable = disable;
}/*CObjServer::disable*/

void CObjServer::server_root_set(CObjPersistent *object) {
   this->server_root = object;
}/*CObjServer::server_root_set*/

int CObjServer::object_root_count(CObjPersistent *object) {
   CObjPersistent *root;
   int count = 0;

   while (object) {
      root = this->server_root;
      if (object == root) {
         count++;
      }
      object = CObjPersistent(CObject(object).parent());
   }

   return count;
}/*CObjServer::object_root_count*/

CThread *CObjServer::thread_owner_find(CObjPersistent *object) {
   CThread *result = CFramework(&framework).thread_main();

   return result;
}/*CObjServer::thread_owner_find*/

void CObjServer::client_add(CObjClient *client) {
   ARRAY(&this->client).item_add(client);
}/*CObjServer::client_add*/

void CObjServer::client_remove(CObjClient *client) {
   ARRAY(&this->client).item_remove(client);
}/*CObjServer::client_remove*/

void CObjServer::attribute_update(CObjPersistent *object,
                                  ARRAY<const TAttribute *> *attribute) {
}/*CObjServer::attribute_update*/

void CObjServer::child_add(CObjPersistent *object, CObjPersistent *child) {
   CObjClient *client;
   int i;

   if (this->disable)
      return;

   CObjServer(this).attribute_update(object, NULL);

   for (i = 0; i < ARRAY(&this->client).count(); i++) {
      client = ARRAY(&this->client).data()[i];

      if (CObjClient(client).object_subscribed(CObject(object))) {
         CObjClient(client).notify_object_child_add(object, child);
      }
   }

   CObjServer(this).attribute_update(object, NULL);
}/*CObjServer::child_add*/

void CObjServer::child_remove(CObjPersistent *object, CObjPersistent *child) {
   CObjClient *client;
   int i, count;

   if (this->disable)
     return;

   CObjServer(this).attribute_update(object, NULL);

   for (i = 0; i < ARRAY(&this->client).count(); i++) {
      count = ARRAY(&this->client).count();
      client = ARRAY(&this->client).data()[i];

      if (CObjClient(client).object_subscribed(CObject(/*>>>object*/child))) {
         CObjClient(client).notify_object_child_remove(object, CObjPersistent(child));
      }
      
      /* notification has caused a client to be removed, reset out iterator */
      if (count != ARRAY(&this->client).count()) {
         i--;
         count = ARRAY(&this->client).count();
      }
   }
}/*CObjServer::child_remove*/

void CObjServer::selection_update(CObjPersistent *object, bool changing) {
   CObjClient *client;
   int i;

   if (this->disable)
      return;

   for (i = 0; i < ARRAY(&this->client).count(); i++) {
      client = ARRAY(&this->client).data()[i];

      if (CObjClient(client).object_subscribed(CObject(object))) {
         CObjClient(client).notify_selection_update(object, changing);
      }
   }
}/*CObjServer::selection_update*/

void CObjServer::root_selection_update(void) {
   CObjPersistent(this->server_root).notify_selection_update(TRUE);
}/*CObjServer::root_update*/

void CObjServer::root_selection_update_end(void) {
   CObjPersistent(this->server_root).notify_selection_update(FALSE);
}/*CObjServer::root_update_end*/

void CObjPersistent::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CObjServer *server;

   if (!CObjClass_is_derived(&class(CObjPersistent), CObject(object).obj_class())) {
      return;
   }

   server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(object));

   if (server && server->disable)
      return;

   if (server) {
      switch (linkage) {
      case EObjectLinkage.ChildAdd:
         CObjServer(server).child_add(this, CObjPersistent(object));
         break;
      case EObjectLinkage.ChildRemovePre:
         CObjServer(server).child_remove(this, CObjPersistent(object));
         break;
      default:
         break;
      }
   }
}/*CObjPersistent::notify_object_linkage*/

void CObjPersistent::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObjClient *client;
   int i;
   CObjServer *server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(this));

   if (server && server->disable)
      return;

   if (server/* && !changing*/) {
      for (i = 0; i < ARRAY(&server->client).count(); i++) {
         client = ARRAY(&server->client).data()[i];

         if (CObjClient(client).object_subscribed(CObject(this))) {
            CObjClient(client).notify_object_attribute_update(this, NULL, changing);
         }
      }

      CObjServer(server).attribute_update(this, attribute);
   }
}/*CObjPersistent::notify_attribute_update*/

void CObjPersistent::notify_selection_update(bool changing) {
   CObjServer *server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(this));

   if (server && server->disable)
     return;

   if (server) {
      CObjServer(server).selection_update(this, changing);
   }
}/*CObjPersistent::notify_selection_update*/

void CObjPersistent::notify_all_update(bool changing) {
   CObjClient *client;
   int i;
   CObjServer *server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(this));

   if ((server && server->disable) || changing)
      return;

   if (server) {
      for (i = ARRAY(&server->client).count() - 1; i >= 0; i--) {
         client = ARRAY(&server->client).data()[i];

         if (CObjClient(client).object_subscribed(CObject(this))) {
            CObjClient(client).notify_all_update();
         }
      }
   }
}/*CObjPersistent::notify_all_update*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

