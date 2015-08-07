extern TTemplate tp_Module;

extern TTemplate tp_List;
extern TTemplate tp_Array;
extern TTemplate tp_String;
extern TTemplate tp_Stack;
extern TTemplate tp_Bitfield;
extern TTemplate tp_Vector;

extern TTemplate tp_Class;
extern TTemplate tp_Attribute;
extern TTemplate tp_Element;
extern TTemplate tp_Menu;
extern TTemplate tp_Data;
extern TTemplate tp_Alias;
extern TTemplate tp_Option;
extern TTemplate tp_Object;
extern TTemplate tp_Base;
extern TTemplate tp_New;
extern TTemplate tp_Delete;

extern TTemplate tp_Enum;
extern TTemplate tp_Text;

extern TTemplate tp_Try;
extern TTemplate tp_Catch;
extern TTemplate tp_Finally;
extern TTemplate tp_Throw;
extern TTemplate tp_Exception;

#define EXCEPTION_NAME "EXCEPTION"
bool CLASS_in_class(void);
bool CLASS_in_attribute(void);
void MODULE_current_file(char *cur_filename, bool current_file);
int TEXT_enum_count(char *name);
