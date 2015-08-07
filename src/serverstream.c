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
#include "objpersistent.c"
#include "objcontainer.c"
#include "syncobject.c"
#include "objdataserver.c"
#include "event.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#define DUMP_DEBUG FALSE

class CObjClientStream : CObjClient {
 private:
   CObjPersistent *resolving;
   CIOObject *connection;

   void event_add(CEventObjPersistent *event);
   void event_attribute_add(CObjPersistent *object, const TAttribute *attribute, int index);

   void object_add(CObjPersistent *object, CObjPersistent *child);
   void object_send(CObjPersistent *object);

   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute);

#if 0
   virtual void stream_object_new(void);
   virtual void stream_object_delete(void);
   virtual void stream_object_set(void);
#endif
 public:
   void CObjClientStream(CObjServer *server, CObject *host, CIOObject *connection);
   void process_pending(void);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CObjClientStream::CObjClientStream(CObjServer *server, CObject *host, CIOObject *connection) {
   this->connection = connection;

   CObjClient(this).CObjClient(server, host);
}/*CObjClientStream::CObjClientStream*/

void CObjClientStream::process_pending(void) {
   CString message;
   CEventObjPersistent event;
   CString type_string;
   CString path_string, value;
   CXPath path;

   CObjPersistent *object;
   const CObjClass *obj_class;

   new(&message).CString(NULL);

   new(&type_string).CString(NULL);
   new(&path_string).CString(NULL);
   new(&value).CString(NULL);

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);

   /* receive message */
   CIOObject(this->connection).read_string(&message);
#if DUMP_DEBUG
   printf("<%s\n", CString(&message).string());
#endif
//   CObjServer(CObjClient(this)->server).client_active_set(CObjClient(this));

   CString(&message).tokenise(&type_string, ":", TRUE);
   CString(&message).tokenise(&path_string, ":", TRUE);
   CString(&message).tokenise(&value, ":", TRUE);

   new(&event).CEventObjPersistent(ENUM:decode(EEventObjPersistent,
                                               CString(&type_string).string()),
                                   CString(&path_string).string(),
                                   CString(&value).string());
//   printf("message: %d#%s#%s\n", event.type,
//          CString(&event.path).string(), CString(&event.value).string());

   CXPath(&path).path_set(CString(&path_string).string());

   this->resolving = path.object.object;
   switch(event.type) {
   case EEventObjPersistent.AllDelete:
      break;
   case EEventObjPersistent.ObjectNew:
      obj_class = CFramework(&framework).obj_class_find(CString(&event.value).string());
      if (!obj_class) {
         ASSERT("CObjServerStream: class not found");
      }
      if (!CObjClass_is_derived(&class(CObjPersistent), obj_class)) {
         ASSERT("CObjServerStream: non persistent class specified");
      }

      object = CObjPersistent(new.class(obj_class));
      CObjPersistent(object).CObjPersistent();
      CObject(path.object.object).child_add(CObject(object));
      break;
   case EEventObjPersistent.ObjectDelete:
      delete(path.object.object);
      break;
   case EEventObjPersistent.AttributeSet:
      if (path.object.object) {
         CObjPersistent(path.object.object).attribute_set_text(path.object.attribute,
                                                               CString(&event.value).string());
         CObjPersistent(path.object.object).attribute_update_end();
      }
      break;
   case EEventObjPersistent.AttributeUpdateEnd:
      break;
   case EEventObjPersistent.SelectionUpdate:
      break;
   default:
      break;
   }

   this->resolving = NULL;
//   CObjServer(CObjClient(this)->server).client_active_set(NULL);

   delete(&event);

   delete(&path);

   delete(&value);
   delete(&path_string);
   delete(&type_string);

   delete(&message);
}/*CObjClientStream::process_pending*/

void CObjClientStream::event_add(CEventObjPersistent *event) {
   CString message;

   if (this->connection) {
      new(&message).CString(NULL);

      CString(&message).printf("%s", EEventObjPersistent(event->type).name());
      if (!CString(&event->path).empty()) {
         CString(&message).printf_append(":%s", CString(&event->path).string());
      }
      if (!CString(&event->value).empty()) {
         CString(&message).printf_append(":%s", CString(&event->value).string());
      }
#if DUMP_DEBUG
      printf(">%s\n", CString(&message).string());
#endif
      CIOObject(this->connection).write_string(CString(&message).string());

      delete(&message);
   }

   delete(event);
}/*CObjClientStream::event_add*/

void CObjClientStream::event_attribute_add(CObjPersistent *object, const TAttribute *attribute, int index) {
   CEventObjPersistent event;
   CXPath path;
   CString path_string, value;

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);
   new(&path_string).CString(NULL);
   new(&value).CString(NULL);

   CXPath(&path).set(object, attribute, index);
   CXPath(&path).path_get(&path_string);
   CObjPersistent(object).attribute_get_string(attribute, &value);

   new(&event).CEventObjPersistent(EEventObjPersistent.AttributeSet,
                                   CString(&path_string).string(),
                                   CString(&value).string());

   CObjClientStream(this).event_add(&event);

   delete(&value);
   delete(&path_string);
   delete(&path);
}/*CObjClientStream::event_attribute_add*/

void CObjClientStream::object_add(CObjPersistent *object, CObjPersistent *child) {
   CXPath path;
   CString path_string;
   CEventObjPersistent event;

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);
   new(&path_string).CString(NULL);

   CXPath(&path).set(object, NULL, 0);
   CXPath(&path).path_get(&path_string);

   new(&event).CEventObjPersistent(EEventObjPersistent.ObjectNew,
                                   CString(&path_string).string(),
                                   CObjClass_alias(CObject(child).obj_class()));
   CObjClientStream(this).event_add(&event);

   CObjClientStream(this).object_send(child);

   delete(&path_string);
   delete(&path);
}/*CObjClientStream::object_add*/

void CObjClientStream::object_send(CObjPersistent *object) {
   int i;
   ARRAY<const TAttribute *> attr_list;

   /* send current state of all of objects attributes */
   ARRAY(&attr_list).new();
   CObjPersistent(object).attribute_list(&attr_list, TRUE, FALSE, FALSE);
   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      if (!CObjPersistent(object).attribute_default_get(ARRAY(&attr_list).data()[i])) {
         CObjClientStream(this).event_attribute_add(object, ARRAY(&attr_list).data()[i], -1);
      }
   }
   ARRAY(&attr_list).delete();
}/*CObjClientStream::object_send*/

void CObjClientStream::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {
   if (this->resolving == object)
      return;

   CObjClientStream(this).object_add(object, child);
}/*CObjClientStream::notify_object_child_add*/

void CObjClientStream::notify_all_update(void) {
   CObject *object;
   CObjPersistent *object_root;
   CEventObjPersistent event;
   int i;

   new(&event).CEventObjPersistent(EEventObjPersistent.AllDelete, NULL, NULL);
   CObjClientStream(this).event_add(&event);

   for (i = 0; i < ARRAY(&CObjClient(this)->object_root).count(); i++) {
      object_root = ARRAY(&CObjClient(this)->object_root).data()[i];

      object = CObject(object_root).child_first();

      while (object) {
         CObjClientStream(this).object_add(object_root, CObjPersistent(object));
         object = CObject(object_root).child_next(object);
      }
   }
}/*CObjClientStream::notify_all_update*/

void CObjClientStream::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {
   CXPath path;
   CString path_string;
   CEventObjPersistent event;

   if (this->resolving == child)
      return;

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);
   new(&path_string).CString(NULL);

   CXPath(&path).set(child, NULL, 0);
   CXPath(&path).path_get(&path_string);

   new(&event).CEventObjPersistent(EEventObjPersistent.ObjectDelete,
                                   CString(&path_string).string(),
                                   NULL);
   CObjClientStream(this).event_add(&event);

   delete(&path_string);
   delete(&path);
}/*CObjClientStream::notify_object_child_remove*/

void CObjClientStream::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute) {
   if (this->resolving == object &&
      CObjServer(CObjClient(this)->server).object_root_count(object) <= 1) {
      return;
   }

   CObjClientStream(this).object_send(object);
}/*CObjClientStream::notify_object_attribute_update*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

