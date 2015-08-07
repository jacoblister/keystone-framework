/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CStateTest : CApplication {
 private:    
   /* State Machine*/
   CFsm fsm;
 
   STATE state_A(CEvent *event);
      STATE state_A1(CEvent *event); 
      STATE state_A2(CEvent *event); 
         STATE state_A22(CEvent *event); 
   STATE state_B(CEvent *event);
      STATE state_B1(CEvent *event);
      STATE state_B2(CEvent *event);
         STATE state_B22(CEvent *event);  
 public:
   ALIAS<"statetest">;     
 
   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CStateTest, statetest>;

STATE CStateTest::state_A(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State A: Enter\n");
         break;
      case EEventState.leave:
         printf("State A: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CFsm:top;
}/*CStateTest::state_A*/

STATE CStateTest::state_A1(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State A1: Enter\n");
         break;
      case EEventState.leave:
         printf("State A1: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CStateTest:state_A;
}/*CStateTest::state_A1*/

STATE CStateTest::state_A2(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State A2: Enter\n");
         break;
      case EEventState.leave:
         printf("State A2: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CStateTest:state_A;
}/*CStateTest::state_A2*/

STATE CStateTest::state_A22(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State A22: Enter\n");
         break;
      case EEventState.leave:
         printf("State A22: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CStateTest:state_A2;
}/*CStateTest::state_A22*/

STATE CStateTest::state_B(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State B: Enter\n");
         break;
      case EEventState.leave:
         printf("State B: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CFsm:top;
}/*CStateTest::state_B*/

STATE CStateTest::state_B1(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State B1: Enter\n");
         break;
      case EEventState.leave:
         printf("State B1: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CStateTest:state_B;
}/*CStateTest::state_B1*/

STATE CStateTest::state_B2(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State B2: Enter\n");
         break;
      case EEventState.leave:
         printf("State B2: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CStateTest:state_B;
}/*CStateTest::state_B2*/

STATE CStateTest::state_B22(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         printf("State B22: Enter\n");
         break;
      case EEventState.leave:
         printf("State B22: Leave\n");
         break;
      default:
	 break;
      }
   }

   return (STATE)&CStateTest:state_B2;
}/*CStateTest::state_B22*/

int CStateTest::main(ARRAY<CString> *args) {
   new(&this->fsm).CFsm(CObject(this), (STATE)&CStateTest::state_B2);
   printf("INIT\n");    
   CFsm(&this->fsm).init();
   printf("B2->A2\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_A2);
   printf("A2->A\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_A);
   printf("A->A1\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_A1);
   printf("A1->A2\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_A2);
   printf("A2->B\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_B);
   printf("B->B\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_B);    
   printf("B->A22\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_A22);    
   printf("A22->B22\n");
   CFsm(&this->fsm).transition((STATE)&CStateTest::state_B22);    
   return 0;
}/*CStateTest::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/