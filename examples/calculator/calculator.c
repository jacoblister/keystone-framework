/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

ENUM:typedef<ECalcState> {
   {operandAInit},
   {operandAZero},
   {operandA},
   {operandADecimal},
   {operandBInit},
   {operandBZero},
   {operandB},
   {operandBDecimal},
};

class CGWindowCalc : CGWindow {
 private:
   bool notify_request_close(void);
 public:
   void CGWindowCalc(CGCanvas *layout);
};


class CGLayoutCalc : CGLayout {
 private:
   /*STATE state_animate()*/
      STATE state_animate_calc(CEvent *event);
 public:
   ALIAS<"svg">;
   void CGLayoutCalc(CObjServer *obj_server, CObjPersistent *data_source);
};

class CCalculator : CApplication {
 private:
   CGWindowCalc window;
   CGLayoutCalc layout;

   double operandA;
   double operandB;
   char operator;

   ECalcState state;

   void new(void);
   void delete(void);

   bool evaluate(double *result);
   void event(CEvent *event);
 public:
   ALIAS<"calculator">;
   CObjServer obj_server;

   MENU<bool file_quit, "/File/Quit"> {
      CFramework(&framework).kill();
   };
   MENU<bool skin_svg, "/Skin/SVG"> {
      CCalculator(this).skin_load("memfile:skin_svg.svg");
   };
   MENU<bool skin_svg_curves, "/Skin/SVG Curves"> {
      CCalculator(this).skin_load("memfile:skin_svg_curves.svg");
   };
   MENU<bool skin_xul, "/Skin/XUL"> {
      CCalculator(this).skin_load("memfile:skin_xul.svg");
   };
   MENU<bool skin_hybrid, "/Skin/Hybrid"> {
      CCalculator(this).skin_load("memfile:skin_hybrid.svg");
   };
   MENU<bool help_about, "/Help/About"> {
      CGLayout layout_about;
      CGWindowDialog dialog;
      CGLicenseFill license_fill;

      new(&layout_about).CGLayout(0, 0, NULL, NULL);

      new(&license_fill).CGLicenseFill();
      CGLicenseFill(&license_fill).fill_layout(&layout_about);
      delete(&license_fill);

      new(&dialog).CGWindowDialog("About Keystone Calculator", CGCanvas(&layout_about), NULL);
      CGWindowDialog(&dialog).button_add("OK", 0);
      CGWindowDialog(&dialog).execute();
      delete(&dialog);
   };

   ATTRIBUTE<char input> {
      CEventKey event;

      this->input = *data;

      new(&event).CEventKey(EEventKeyType.down, EEventKey.Ascii, this->input, 0);
      CCalculator(this).event(CEvent(&event));
      delete(&event);
   };


   ATTRIBUTE<CString value>;
   CString input_value;

   void main(ARRAY<CString> *args);
   void skin_load(const char *name);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#define CASE_KEY_1_9  case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
#define CASE_KEY_OPERATOR case '+': case '-': case '*': case '/'

OBJECT<CCalculator, calculator>;

void CGWindowCalc::CGWindowCalc(CGCanvas *layout) {
   CGWindow(this).CGWindow("Keystone Calculator", layout, NULL);
}/*CGWindowCalc::CGWindowCalc*/

bool CGWindowCalc::notify_request_close(void) {
   CFramework(&framework).kill();
   return FALSE;
}/*CGWindowCalc::notify_close*/

void CGLayoutCalc::CGLayoutCalc(CObjServer *obj_server, CObjPersistent *data_source) {
    CGLayout(this).CGLayout(0, 0, obj_server, data_source);
}/*CGLayoutCalc::CGLayoutCalc*/

STATE CGLayoutCalc::state_animate_calc(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventKey)) {
       if (CEventKey(event)->key == EEventKey.Ascii) {
          CObjPersistent(&calculator).attribute_set_int(ATTRIBUTE<CCalculator,input>, CEventKey(event)->value);
       }
   }
   return (STATE)&CGLayout::state_animate;
}/*CGLayoutCalc::state_animate_calc*/

void CCalculator::new(void) {
   class:base.new();

   new(&this->input).CString(NULL);
   new(&this->value).CString(NULL);
   new(&this->input_value).CString(NULL);
}/*CCalculator::new*/

int CCalculator::main(ARRAY<CString> *args) {
   new(&this->obj_server).CObjServer(CObjPersistent(this));

   new(&this->layout).CGLayoutCalc(&this->obj_server, CObjPersistent(this));
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayoutCalc::state_animate_calc);

   CCalculator(this).skin_load("memfile:skin_svg.svg");

   new(&this->window).CGWindowCalc(CGCanvas(&this->layout));
   CGWindow(&this->window).object_menu_add(CObjPersistent(this), TRUE);
   CGObject(&this->window).show(TRUE);
   CGObject(&this->layout).show(TRUE);

   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayoutCalc::state_animate_calc);

   CObjPersistent(this).attribute_update(ATTRIBUTE<CCalculator,value>);
   CString(&this->value).printf("0");
   CObjPersistent(this).attribute_update_end();

   CFramework(&framework).main();

   return 0;
}/*CCalculator::main*/

void CCalculator::skin_load(const char *name) {
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);
   CGLayout(&this->layout).load_svg_file(name, NULL);
   CGObject(&this->layout).show(TRUE);
   CObjPersistent(&this->layout).attribute_update(NULL);
   CObjPersistent(&this->layout).attribute_update_end();
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayoutCalc::state_animate_calc);
}/*CCalculator::skin_load*/

void CCalculator::delete(void) {
   delete(&this->input);
   delete(&this->value);

   class:base.delete();
}/*CCalculator::delete*/

bool CCalculator::evaluate(double *result) {
   switch (this->operator) {
   case '+':
      *result = this->operandA + this->operandB;
      break;
   case '-':
      *result = this->operandA - this->operandB;
      break;
   case '*':
      *result = this->operandA * this->operandB;
      break;
   case '/':
      if (this->operandB != 0.0) {
         *result = this->operandA / this->operandB;
      }
      else {
         return FALSE;
      }
      break;
   default:
      ASSERT("error");
   }

   if (-1.0e10 < *result && *result < 1.0e10) {
      return TRUE;
   }

   return FALSE;
}

void CCalculator::event(CEvent *event) {
   double result;

   switch (toupper(CEventKey(event)->value)) {
   case '.':
      switch (this->state) {
      case ECalcState.operandAInit:
         CString(&this->input_value).printf("0");
      case ECalcState.operandAZero:
      case ECalcState.operandA:
         CString(&this->input_value).printf_append(".");
         this->state = ECalcState.operandADecimal;
         break;
      case ECalcState.operandBInit:
         CString(&this->input_value).printf("0");
      case ECalcState.operandBZero:
      case ECalcState.operandB:
         CString(&this->input_value).printf_append(".");
         this->state = ECalcState.operandBDecimal;
      break;
      default:
         break;
      }
      break;
   case '0':
      switch (this->state) {
      case ECalcState.operandAInit:
         CString(&this->input_value).printf("0");
         this->state = ECalcState.operandAZero;
         break;
      case ECalcState.operandAZero:
         break;
      case ECalcState.operandBInit:
         CString(&this->input_value).printf("0");
         this->state = ECalcState.operandBZero;
         break;
      case ECalcState.operandBZero:
         break;
      default:
         goto key1_9;
      }
      break;
   key1_9:
   CASE_KEY_1_9:
      if (this->state == ECalcState.operandAInit) {
         CString(&this->input_value).clear();
         this->state = ECalcState.operandA;
      }
      if (this->state == ECalcState.operandBInit) {
         CString(&this->input_value).clear();
         this->state = ECalcState.operandB;
      }

      if (CString(&this->input_value).length() < 10) {
         CString(&this->input_value).printf_append("%c", CEventKey(event)->value);
      }
      break;
   CASE_KEY_OPERATOR:
      switch (this->state) {
      case ECalcState.operandAZero:
      case ECalcState.operandA:
      case ECalcState.operandADecimal:
         this->operandA = atof(CString(&this->input_value).string());
      case ECalcState.operandAInit:
         this->operator = CEventKey(event)->value;

         CString(&this->input_value).printf("0");
         this->state = ECalcState.operandBInit;
         return;
      case ECalcState.operandBInit:
         this->operator = CEventKey(event)->value;
         return;
      case ECalcState.operandBZero:
      case ECalcState.operandB:
      case ECalcState.operandBDecimal:
         this->operandB = atof(CString(&this->input_value).string());
         if (CCalculator(this).evaluate(&result)) {
            this->operandA = result;
            CObjPersistent(this).attribute_update(ATTRIBUTE<CCalculator,value>);
            CString(&this->value).printf("%24.11g", this->operandA);
            CObjPersistent(this).attribute_update_end();
         }
         else {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CCalculator,value>);
            CString(&this->value).printf("Error");
            CObjPersistent(this).attribute_update_end();
            this->state = ECalcState.operandAInit;
            return;
         }

         this->operator = CEventKey(event)->value;

         CString(&this->input_value).printf("0");
         this->state = ECalcState.operandBInit;
         return;
      default:
          break;
      }
      break;
   case '=':
      switch (this->state) {
      case ECalcState.operandBZero:
      case ECalcState.operandB:
      case ECalcState.operandBDecimal:
         this->operandB = atof(CString(&this->input_value).string());
         if (CCalculator(this).evaluate(&result)) {
            this->operandA = result;
            CObjPersistent(this).attribute_update(ATTRIBUTE<CCalculator,value>);
            CString(&this->value).printf("%24.11g", this->operandA);
            CObjPersistent(this).attribute_update_end();
         }
         else {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CCalculator,value>);
            CString(&this->value).printf("Error");
            CObjPersistent(this).attribute_update_end();
            this->state = ECalcState.operandAInit;
            return;
         }

         CObjPersistent(this).attribute_update(ATTRIBUTE<CCalculator,value>);
         CString(&this->value).printf("%24.11g", this->operandA);
         CObjPersistent(this).attribute_update_end();

         this->state = ECalcState.operandAInit;

         CString(&this->input_value).printf("0");
         break;
       default:
          break;
      }
      return;
   case 'C':
   case 'E':
      CString(&this->input_value).printf("0");
      this->state = ECalcState.operandAInit;
      break;
   default:
      break;
   }

   CObjPersistent(this).attribute_update(ATTRIBUTE<CCalculator,value>);
   CString(&this->value).set(CString(&this->input_value).string());
   CObjPersistent(this).attribute_update_end();
}/*CCalculator::event*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
