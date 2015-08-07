/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#include "framework.c"
#include "cencode.h"
#include "cdecode.h"

class CBinData : CObject {
 private:    
    ARRAY<byte> data;
    void new(void);
    void ~CBinData(void);
 public:
    void CBinData(void);
};

ATTRIBUTE:typedef<CBinData>;

class CLightDemo : CApplication {
 private:
   void new(void);
   void ~CLightDemo(void);
 public:
   ALIAS<"lightdemo">;
 
   ATTRIBUTE:ARRAY<bool switchState>;
   ATTRIBUTE:ARRAY<byte bytes>;
   ATTRIBUTE<CBinData data>;
 
   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CBinData::new(void) {
   ARRAY(&this->data).new();    
}/*CBinData::new*/

void CBinData::CBinData(void) {
}/*CBinData::CBinData*/

void CBinData::~CBinData(void) {
   ARRAY(&this->data).delete();        
}/*CBinData::~CBinData*/

bool ATTRIBUTE:convert<CBinData>(struct tag_CObjPersistent *object,
                                  const TAttributeType *dest_type, const TAttributeType *src_type,
                                  int dest_index, int src_index,
                                  void *dest, const void *src) {
   CBinData *bindata;
   CString *string;
                                      
   int codelength;
   int plainlength;
   base64_decodestate dec_state;
   base64_encodestate enc_state;

   if (dest_type == &ATTRIBUTE:type<CBinData> && src_type == &ATTRIBUTE:type<CString>) {
      bindata = (CBinData *)dest;
      string = (CString *)src;
       
      base64_init_decodestate(&dec_state);       
      ARRAY(&bindata->data).used_set(CString(string).length());
      plainlength = base64_decode_block(CString(string).string(), CString(string).length(), ARRAY(&bindata->data).data(), &dec_state);
      ARRAY(&bindata->data).used_set(plainlength);       
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<CBinData>) {
      bindata = (CBinData *)src;
      string = (CString *)dest;
       
      base64_init_encodestate(&enc_state);
      ARRAY(&string->data).used_set(ARRAY(&bindata->data).count() * 2);
      codelength = base64_encode_block(ARRAY(&bindata->data).data(), ARRAY(&bindata->data).count(), ARRAY(&string->data).data(), &enc_state);
      codelength += base64_encode_blockend(ARRAY(&string->data).data() + codelength, &enc_state);
      ARRAY(&string->data).data()[codelength] = '\0';
      codelength++;
      ARRAY(&string->data).used_set(codelength);
      return TRUE;
   }
   return FALSE;
}

OBJECT<CLightDemo, lightdemo>;

void CLightDemo::new(void) {
   ARRAY(&this->switchState).new();
   ARRAY(&this->bytes).new();
   new(&this->data).CBinData();
}/*CLightDemo::new*/

void CLightDemo::~CLightDemo(void) {
   delete(&this->data);
   ARRAY(&this->bytes).delete();    
   ARRAY(&this->switchState).delete();
}/*CLightDemo::~CLightDemo*/

int CLightDemo::main(ARRAY<CString> *args) {
   CFile file;    
   CString filename, message;
   CGDialogMessage dialog;

   new(&message).CString(NULL);
   new(&filename).CString(NULL);    
    
   /* read input file chosen on command line and write it back to standard output */
   if (ARRAY(args).count() < 2) {
      CString(&filename).set("lightdemo.xml");
   }
   else {
      CString(&filename).set(CString(&ARRAY(args).data()[1]).string());
   }
   if (CObjPersistent(this).state_xml_load_file(CString(&filename).string(), TRUE, &message)) {
      new(&file).CFile();
      CIOObject(&file).open("output.out", O_CREAT | O_TRUNC | O_RDWR | O_BINARY);
      CIOObject(&file).write(ARRAY(&this->data.data).data(), ARRAY(&this->data.data).count());
      CIOObject(&file).close();
      delete(&file);
       
      new(&file).CFile();
      CIOObject(&file).open_handle(STDOUT);
      CObjPersistent(this).state_xml_store(CIOObject(&file));
      delete(&file);
   }

   if (CString(&message).length()) {
      new(&dialog).CGDialogMessage("Light Demo Error", CString(&message).string(), NULL);
      CGWindowDialog(&dialog).execute();
      delete(&dialog);
   }
   
   delete(&filename);
   delete(&message);

   return 0;
}/*CLightDemo::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/