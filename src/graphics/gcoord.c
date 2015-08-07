/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../framework_base.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

//#ifndef M_PI
//#define M_PI 3.14159265358979323846f
//#endif

typedef double TCoord;
ARRAY:typedef<TCoord>;
ATTRIBUTE:typedef<TCoord>;

typedef struct {
   TCoord x, y;
} TPoint;
ARRAY:typedef<TPoint>;
ATTRIBUTE:typedef<TPoint>;

typedef struct {
   TPoint point[2];
} TRect;
ATTRIBUTE:typedef<TRect>;

static inline bool TRect_overlap(TRect *a, TRect *b) {
   return !(a->point[1].x < b->point[0].x ||
            a->point[0].x > b->point[1].x ||
            a->point[1].y < b->point[0].y ||
            a->point[0].y > b->point[1].y);
}/*TRect_overlap*/

ENUM:typedef<EGTransform> {
   {none}, {matrix}, {translate}, {scale}, {rotate}, {skewX}, {skewY},
};

typedef struct {
   EGTransform type;
   union {
      struct {
         double data[2][3];
      } matrix;
      struct {
         double tx, ty;
         bool ty_implied;
      } translate;
      struct {
         double sx, sy;
         bool sy_implied;
      } scale;
      struct {
         double rotate_angle;
         double cx, cy;
         bool cxy_used;
      } rotate;
      struct {
         double skew_angle;
      } skew;
   } t;
} TGTransform;

ARRAY:typedef<TGTransform>;
ATTRIBUTE:typedef<TGTransform>;

typedef TGTransform TGTransformData;
ATTRIBUTE:typedef<TGTransformData>;

typedef struct {
   double data[3][3];
} TGMatrix;

typedef struct {
   TGMatrix matrix;
   TPoint scaling;
   double rotation;
} TGCoordState;

STACK:typedef<TGCoordState>;

class CGCoordSpace : CObject {
 private:
   TGMatrix matrix;
   TPoint scaling;
   double rotation;
   STACK<TGCoordState> state_stack;

   void new(void);
   void delete(void);

   double vector_difference_angle(TPoint *v1, TPoint *v2);
//   bool transform_invert(TGTransform *transform);
//   bool transform_list_apply(ARRAY<TGTransform> *list, TGTransform *transform);
   
   void matrix_invert(TGMatrix *matrix, TGMatrix *result);
   void matrix_multiply(TGMatrix *matrix, bool prepend);

   void scaling_recalc(void);
 public:
   void CGCoordSpace(void);
   void reset(void);
   void push(void);
   void pop(void);
   void transform_apply(TGTransform *transform, bool prepend);
   void transform_get_current(TGTransform *result);
   void point_array_utov(int count, TPoint *point);
   void point_array_vtou(int count, TPoint *point);
   static inline void point_utov(TPoint *point);
   static inline void point_vtou(TPoint *point);

   double measure_vector(TPoint *vector);
   static inline double scaling_x(void);
   static inline double scaling_y(void);
   static inline double scaling(void);
   static inline double rotation(void);

   void arc_endpoints(double cx, double cy, double rx, double ry,
                      double x_axis_rotation, double th0, double thd,
                      double *out_startx, double *out_starty, 
                      double *out_endx, double *out_endy, 
                      int *out_large_arc_flag, int *out_sweep_flag);
   void arc_position(double curx, double cury, double rx, double ry,
                     double x_axis_rotation, int large_arc_flag, int sweep_flag, double x, double y,
                     double *out_cx, double *out_cy, double *out_rx, double *out_ry,
                     double *out_th0, double *out_thd);

   void extent_reset(TRect *rect);
   bool extent_empty(TRect *rect);
   void extent_add(TRect *rect, int count, TPoint *point);
   void extent_extend(TRect *rect, TCoord extend);

   bool area_inside_rect(ARRAY<TPoint> *area, TRect *rect);
   bool rect_inside_area(TRect *rect, ARRAY<TPoint> *area);
};

static inline void CGCoordSpace::point_utov(TPoint *point) {
   CGCoordSpace(this).point_array_utov(1, point);
}

static inline void CGCoordSpace::point_vtou(TPoint *point) {
   CGCoordSpace(this).point_array_vtou(1, point);
}

static inline double CGCoordSpace::scaling_x(void) {
   return this->scaling.x;
}

static inline double CGCoordSpace::scaling_y(void) {
   return this->scaling.y;
}

static inline double CGCoordSpace::rotation(void) {
   return this->rotation;
}

static inline double CGCoordSpace::scaling(void) {
   return this->scaling.x > this->scaling.y ? this->scaling.y : this->scaling.x;
}
/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <math.h>

#include "../cstring.c"
#include "../objpersistent.c"

bool ATTRIBUTE:convert<TCoord>(CObjPersistent *object,
                                const TAttributeType *dest_type, const TAttributeType *src_type,
                                int dest_index, int src_index,
                                void *dest, const void *src) {
   TCoord *value;
   CString *string;
   int *int_value;
   double *double_value;

   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == NULL) {
      memset(dest, 0, sizeof(TCoord));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<TCoord>) {
      *((TCoord *)dest) = *((TCoord *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TCoord>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (TCoord *)dest;
      string = (CString *)src;
      *value = atof(CString(string).string());
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<int>) {
      value  = (TCoord *)dest;
      int_value = (int *)src;
      *value = *int_value;
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<TCoord>) {
      int_value = (int *)dest;
      value  = (TCoord *)src;
      *int_value = (int)*value;
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<double>) {
      value  = (TCoord *)dest;
      double_value = (double *)src;
      *value = *double_value;
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<double> && src_type == &ATTRIBUTE:type<TCoord>) {
      double_value = (double *)dest;
      value = (TCoord *)src;
      *double_value = (double)*value;
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TCoord>) {
      value  = (TCoord *)src;
      string = (CString *)dest;
      CString(string).printf("%g", *value);
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<TPoint>(CObjPersistent *object,
                               const TAttributeType *dest_type, const TAttributeType *src_type,
                               int dest_index, int src_index,
                               void *dest, const void *src) {
   TPoint *point;
   CString *string;
   const char *cp;

   if (dest_type == &ATTRIBUTE:type<TPoint> && src_type == NULL) {
      memset(dest, 0, sizeof(TPoint));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TPoint> && src_type == &ATTRIBUTE:type<TPoint>) {
      *((TPoint *)dest) = *((TPoint *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TPoint>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TPoint> && src_type == &ATTRIBUTE:type<CString>) {
      point  = (TPoint *)dest;
      string = (CString *)src;
      point->x = atof(CString(string).string());
      point->y = 0;
      cp = strchr(CString(string).string(), ',');
      if (!cp) {
         cp = strchr(CString(string).string(), ' ');
      }
      if (cp) {
         point->y = atof(cp + 1);
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TPoint>) {
      point  = (TPoint *)src;
      string = (CString *)dest;
      CString(string).printf("%g,%g", point->x, point->y);
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<TRect>(CObjPersistent *object,
                              const TAttributeType *dest_type, const TAttributeType *src_type,
                              int dest_index, int src_index,
                              void *dest, const void *src) {
   TRect *rect;
   CString *string;
   TCoord value;

   if (dest_type == &ATTRIBUTE:type<TRect> && src_type == NULL) {
      memset(dest, 0, sizeof(TRect));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TRect> && src_type == &ATTRIBUTE:type<TRect>) {
      *((TRect *)dest) = *((TRect *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TRect>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TRect> && src_type == &ATTRIBUTE:type<CString>) {
      rect   = (TRect *)dest;
      string = (CString *)src;
      if (dest_index == -1) {
         sscanf(CString(string).string(), "%lf %lf %lf %lf",
                &rect->point[0].x, &rect->point[0].y, &rect->point[1].x, &rect->point[1].y);
      }
      else {
         value = atof(CString(string).string());
         switch (dest_index) {
         case 0:
            rect->point[0].x = value;
            break;
         case 1:
            rect->point[0].y = value;
            break;
         case 2:
            rect->point[1].x = value;
            break;
         case 3:
            rect->point[1].y = value;
            break;
         }

      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TRect>) {
      rect   = (TRect *)src;
      string = (CString *)dest;
      if (src_index == -1) {
         CString(string).printf("%g %g %g %g", rect->point[0].x, rect->point[0].y,
                                                rect->point[1].x, rect->point[1].y);
      }
      else {
         switch (src_index) {
         case 0:
            value = rect->point[0].x;
            break;
         case 1:
            value = rect->point[0].y;
            break;
         case 2:
            value = rect->point[1].x;
            break;
         case 3:
            value = rect->point[1].y;
            break;
         default:
            value = 0;
            break;
         }
         CString(string).printf("%g", value);
      }
      return TRUE;
   }
   return FALSE;
}

static bool TGTransformData_string_read(TGTransform *transform, CString *string) {
   int i, j;
   CString value;
   bool result;

   new(&value).CString(NULL);

   result = TRUE;
   switch (transform->type) {
   case EGTransform.none:
      break;
   case EGTransform.matrix:
      for (j = 0; j < 2; j++) {
         for (i = 0; i < 3; i++) {
            if (!result || !CString(string).tokenise(&value, ",", TRUE)) {
               result = FALSE;
               break;
            }
            transform->t.matrix.data[j][i]  = (double) atof(CString(&value).string());
         }
      }
      break;
   case EGTransform.translate:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.translate.tx = (double) atof(CString(&value).string());
      if (CString(string).tokenise(&value, ",", TRUE)) {
         transform->t.translate.ty = (double) atof(CString(&value).string());
         transform->t.translate.ty_implied = FALSE;
      }
      else {
         transform->t.translate.ty = 0;
         transform->t.translate.ty_implied = TRUE;
      }
      break;
   case EGTransform.scale:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.scale.sx = (double) atof(CString(&value).string());
      if (CString(string).tokenise(&value, ",", TRUE)) {
         transform->t.scale.sy = (double) atof(CString(&value).string());
         transform->t.scale.sy_implied = FALSE;
      }
      else {
         transform->t.scale.sy = transform->t.scale.sx;
         transform->t.scale.sy_implied = TRUE;
      }
      break;
   case EGTransform.rotate:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.rotate.rotate_angle = (double) atof(CString(&value).string());
      if (CString(string).tokenise(&value, ",", TRUE)) {
         transform->t.rotate.cx = (double) atof(CString(&value).string());
         if (CString(string).tokenise(&value, ",", TRUE)) {
            transform->t.rotate.cy = (double) atof(CString(&value).string());
         }
         else {
            result = FALSE;
            break;
         }
      transform->t.rotate.cxy_used = TRUE;
      }
      else {
         transform->t.rotate.cxy_used = FALSE;
      }
      break;
   case EGTransform.skewX:
   case EGTransform.skewY:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.skew.skew_angle = (double) atof(CString(&value).string());
      break;
   default:
      result = FALSE;
      break;
   }

   delete(&value);

   return result;
}/*TGTransformData_string_read*/

static void TGTransformData_string_write(TGTransform *transform, CString *string) {
   switch (transform->type) {
   case EGTransform.none:
      break;
   case EGTransform.matrix:
      CString(string).printf_append("%g,%g,%g,%g,%g,%g",
                                    transform->t.matrix.data[0][0], transform->t.matrix.data[0][1], transform->t.matrix.data[0][2],
                                    transform->t.matrix.data[1][0], transform->t.matrix.data[1][1], transform->t.matrix.data[1][2]);
      break;
   case EGTransform.translate:
      CString(string).printf_append("%g", transform->t.translate.tx);
      if (!transform->t.translate.ty_implied) {
         CString(string).printf_append(",%g", transform->t.translate.ty);
      }
      break;
   case EGTransform.scale:
      CString(string).printf_append("%g", transform->t.scale.sx);
      if (!transform->t.scale.sy_implied) {
         CString(string).printf_append(",%g", transform->t.scale.sy);
      }
      break;
   case EGTransform.rotate:
      CString(string).printf_append("%g", transform->t.rotate.rotate_angle);
      if (transform->t.rotate.cxy_used) {
         CString(string).printf_append(",%g,%g",
                                       transform->t.rotate.cx, transform->t.rotate.cy);
      }
      break;
   case EGTransform.skewX:
      CString(string).printf_append("%g", transform->t.skew.skew_angle);
      break;
   case EGTransform.skewY:
      CString(string).printf_append("%g", transform->t.skew.skew_angle);
      break;
   }
}/*TGTransformData_string_write*/

bool ATTRIBUTE:convert<TGTransform>(CObjPersistent *object,
                                    const TAttributeType *dest_type, const TAttributeType *src_type,
                                    int dest_index, int src_index,
                                    void *dest, const void *src) {
   TGTransform *transform;
   CString *string, data;
   char *t_begin, *t_end;
   bool result;
   int *int_value;

   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == NULL) {
      memset(dest, 0, sizeof(TGTransform));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == &ATTRIBUTE:type<TGTransform>) {
      *((TGTransform *)dest) = *((TGTransform *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGTransform>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == &ATTRIBUTE:type<int>) {
      transform = (TGTransform *)dest;
      int_value = (int *)src;

      switch (transform->type) {
      case EGTransform.translate:
         switch (dest_index) {
         case 0:
            transform->t.translate.tx = (double) *int_value;
            return TRUE;
         case 1:
            transform->t.translate.ty = (double) *int_value;
            return TRUE;
         }
      default:
         break;
      }
      return FALSE;
   }

   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == &ATTRIBUTE:type<CString>) {
      transform = (TGTransform *)dest;
      string = (CString *)src;
      //sscanf(CString(string).string(), "%d,%d", &point->x, &point->y);
      t_begin = CString(string).strchr('(');
      t_end   = CString(string).strchr(')');
      if (!t_begin || !t_end) {
         return FALSE;
      }

      new(&data).CString(NULL);
      CString(string).extract(&data, CString(string).string(), t_begin - 1);
      transform->type = ENUM:decode(EGTransform, CString(&data).string());
      CString(string).extract(&data, t_begin + 1, t_end - 1);

      result = TGTransformData_string_read(transform, &data);
      delete(&data);
      return result;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGTransform>) {
      transform = (TGTransform *)src;
      string = (CString *)dest;

      CString(string).printf("%s", EGTransform(transform->type).name());
      /*>>>printf_append()?*/

      CString(string).printf_append("(");

      TGTransformData_string_write(transform, string);

      CString(string).printf_append(")");
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<TGTransformData>(CObjPersistent *object,
                                        const TAttributeType *dest_type, const TAttributeType *src_type,
                                        int dest_index, int src_index,
                                        void *dest, const void *src) {
   TGTransform *transform;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<TGTransformData> && src_type == NULL) {
      memset(dest, 0, sizeof(TGTransformData));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGTransformData> && src_type == &ATTRIBUTE:type<TGTransformData>) {
      memcpy(dest, src, sizeof(TGTransformData));
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGTransformData>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGTransformData> && src_type == &ATTRIBUTE:type<CString>) {
      transform = (TGTransform *)dest;
      string = (CString *)src;

      return TGTransformData_string_read(transform, string);
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGTransformData>) {
      transform = (TGTransform *)src;
      string = (CString *)dest;

      TGTransformData_string_write(transform, string);
      return TRUE;
   }
   return FALSE;
};

void CGCoordSpace::new(void) {
   STACK(&this->state_stack).new();
}/*CGCoordSpace::new*/

void CGCoordSpace::CGCoordSpace(void) {
   CGCoordSpace(this).reset();
}/*CGCoordSpace::CGCoordSpace*/

void CGCoordSpace::delete(void) {
   STACK(&this->state_stack).delete();
}/*CGCoordSpace::delete*/

void CGCoordSpace::reset(void) {
   /*identity matrix*/
   this->matrix.data[0][0] = 1; this->matrix.data[0][1] = 0; this->matrix.data[0][2] = 0;
   this->matrix.data[1][0] = 0; this->matrix.data[1][1] = 1; this->matrix.data[1][2] = 0;
   this->matrix.data[2][0] = 0; this->matrix.data[2][1] = 0; this->matrix.data[2][2] = 1;

   this->rotation = 0;
}/*CGCoordSpace::reset*/

void CGCoordSpace::push(void) {
   TGCoordState state;

   state.matrix = this->matrix;
   state.scaling = this->scaling;
   state.rotation = this->rotation;

   STACK(&this->state_stack).push(state);
}/*CGCoordSpace::push*/

void CGCoordSpace::pop(void) {
   TGCoordState state;

   state = STACK(&this->state_stack).pop();
   this->rotation  = state.rotation;   
   this->scaling = state.scaling;   
   this->matrix = state.matrix;
}/*CGCoordSpace::pop*/

double CGCoordSpace::vector_difference_angle(TPoint *v1, TPoint *v2) {
   double result;
   
   result = (atan2(v2->y,v2->x) - atan2(v1->y,v1->x)) * (360 / (2*PI));
   
   return result;
}/*CGCoordSpace::vector_difference_angle*/

void CGCoordSpace::matrix_invert(TGMatrix *matrix, TGMatrix *result) {
   int i, j, k;
   double e;

   memcpy(result, matrix, sizeof(TGMatrix));

   for (k = 0; k < 3; k++) {
      e = result->data[k][k];
      result->data[k][k] = 1.0;
      if (e == 0.0) {
         ASSERT("CGCoordSpace::matrix_invert - Matrix inversion error");
       }
      for (j = 0; j < 3; j++) {
         result->data[j][k] = result->data[j][k] / e;
      }
      for (i = 0; i < 3; i++) {
         if (i != k) {
            e = result->data[k][i];
            result->data[k][i] = 0.0;
            for (j = 0; j < 3; j++) {
               result->data[j][i] = result->data[j][i] - e * result->data[j][k];
            }
         }
      }
   }
}/*CGCoordSpace::matrix_invert*/

void CGCoordSpace::matrix_multiply(TGMatrix *matrix, bool prepend) {
   int x, y, i;
   TGMatrix result;

   CLEAR(&result);

   for (y = 0; y < 3; y++) {
      for (x = 0; x < 3; x++) {
         for (i = 0; i < 3; i++) {
            if (prepend) {
               result.data[y][x] += matrix->data[i][x] * this->matrix.data[y][i];
            }
            else {
               result.data[y][x] += this->matrix.data[i][x] * matrix->data[y][i];
            }
         }
      }
   }

   memcpy(&this->matrix, &result, sizeof(TGMatrix));
}/*CGCoordSpace::matrix_multiply*/

void CGCoordSpace::transform_apply(TGTransform *transform, bool prepend) {
   TGMatrix matrix;
   TGTransform temp;

   CLEAR(&matrix);
   matrix.data[2][2] = 1;
   switch (transform->type) {
   case EGTransform.none:
      return;                            /* no operation */
   case EGTransform.matrix:
      matrix.data[0][0] = transform->t.matrix.data[0][0];
      matrix.data[0][1] = transform->t.matrix.data[0][1];
      matrix.data[0][2] = transform->t.matrix.data[0][2];
      matrix.data[1][0] = transform->t.matrix.data[1][0];
      matrix.data[1][1] = transform->t.matrix.data[1][1];
      matrix.data[1][2] = transform->t.matrix.data[1][2];
      break;
   case EGTransform.translate:
      matrix.data[0][0] = 1;
      matrix.data[1][1] = 1;
      matrix.data[0][2] = transform->t.translate.tx;
      matrix.data[1][2] = transform->t.translate.ty;
      break;
   case EGTransform.scale:
      matrix.data[0][0] = transform->t.scale.sx;
      matrix.data[1][1] = transform->t.scale.sy;
      break;
   case EGTransform.rotate:
      temp.type = EGTransform.translate;
      if (transform->t.rotate.cxy_used) {
         temp.t.translate.tx = prepend ? transform->t.rotate.cx : -transform->t.rotate.cx;
         temp.t.translate.ty = prepend ? transform->t.rotate.cy : -transform->t.rotate.cy;
         CGCoordSpace(this).transform_apply(&temp, prepend);
      }
      
      this->rotation += transform->t.rotate.rotate_angle;      

      matrix.data[0][0] = (double)  cos(transform->t.rotate.rotate_angle * (2*PI) / 360);
      matrix.data[0][1] = (double) -sin(transform->t.rotate.rotate_angle * (2*PI) / 360);
      matrix.data[1][0] = (double)  sin(transform->t.rotate.rotate_angle * (2*PI) / 360);
      matrix.data[1][1] = (double)  cos(transform->t.rotate.rotate_angle * (2*PI) / 360);

      CGCoordSpace(this).matrix_multiply(&matrix, prepend);

      if (transform->t.rotate.cxy_used) {
         temp.t.translate.tx = prepend ? -transform->t.rotate.cx : transform->t.rotate.cx;
         temp.t.translate.ty = prepend ? -transform->t.rotate.cy : transform->t.rotate.cy;
         CGCoordSpace(this).transform_apply(&temp, prepend);
      }
      return;
   case EGTransform.skewX:
      matrix.data[0][0] = 1;
      matrix.data[1][1] = 1;
      matrix.data[0][1] = (double) tan(transform->t.skew.skew_angle * (2*PI) / 360);
      break;
   case EGTransform.skewY:
      matrix.data[0][0] = 1;
      matrix.data[1][1] = 1;
      matrix.data[1][0] = (double) tan(transform->t.skew.skew_angle * (2*PI) / 360);
      break;
   }

   CGCoordSpace(this).matrix_multiply(&matrix, prepend);
}/*CGCoordSpace::transform_apply*/

void CGCoordSpace::transform_get_current(TGTransform *result) {
   CLEAR(result);
   result->type = EGTransform.matrix;
   
   result->t.matrix.data[0][0] = this->matrix.data[0][0];
   result->t.matrix.data[0][1] = this->matrix.data[0][1];
   result->t.matrix.data[0][2] = this->matrix.data[0][2];
   result->t.matrix.data[1][0] = this->matrix.data[1][0];
   result->t.matrix.data[1][1] = this->matrix.data[1][1];
   result->t.matrix.data[1][2] = this->matrix.data[1][2];
}/*CGCoordSpace::transform_get_current*/

void CGCoordSpace::scaling_recalc(void) {
   TPoint point[2];

   point[0].x = 0;
   point[0].y = 0;
   point[1].x = 1000;
   point[1].y = 0;
   this->scaling.x = CGCoordSpace(this).measure_vector(point) / 1000;

   point[0].x = 0;
   point[0].y = 0;
   point[1].x = 0;
   point[1].y = 1000;
   this->scaling.y = CGCoordSpace(this).measure_vector(point) / 1000;
}/*CGCoordSpace::scaling_recalc*/

double CGCoordSpace::measure_vector(TPoint *point) {
   double x_length, y_length, length;

   CGCoordSpace(this).point_array_utov(2, point);
   x_length = (double) (point[1].x - point[0].x);
   y_length = (double) (point[1].y - point[0].y);
   length = (double)sqrt((x_length * x_length) + (y_length * y_length));

   return length;
}/*CGCoordSpace::measure_vector*/

void CGCoordSpace::point_array_utov(int count, TPoint *point) {
   TPoint result;

   while (count) {
      result.x = (point->x * this->matrix.data[0][0]) +
                 (point->y * this->matrix.data[0][1]) +
                 (1        * this->matrix.data[0][2]);
      result.y = (point->x * this->matrix.data[1][0]) +
                 (point->y * this->matrix.data[1][1]) +
                 (1        * this->matrix.data[1][2]);
      *point = result;
      point++;
      count--;
   }
}/*CGCoordSpace::point_array_utov*/

void CGCoordSpace::point_array_vtou(int count, TPoint *point) {
   TPoint result;
   TGMatrix inverse;

   CGCoordSpace(this).matrix_invert(&this->matrix, &inverse);

   while (count) {
      result.x = (point->x * inverse.data[0][0]) +
                  (point->y * inverse.data[0][1]) +
                  (1        * inverse.data[0][2]);
      result.y = (point->x * inverse.data[1][0]) +
                  (point->y * inverse.data[1][1]) +
                  (1        * inverse.data[1][2]);
      *point = result;
      point++;
      count--;
   }
}/*CGCoordSpace::point_array_vtou*/

void CGCoordSpace::arc_endpoints(double cx, double cy, double rx, double ry,
                                 double x_axis_rotation, double th0, double thd,
                                 double *out_startx, double *out_starty, 
                                 double *out_endx, double *out_endy, 
                                 int *out_large_arc_flag, int *out_sweep_flag) {
   *out_startx = (sin(th0 * (PI / 180.0)) * rx) + cx;
   *out_starty = -(cos(th0 * (PI / 180.0)) * ry) + cy;

   *out_endx = (sin((th0 + thd) * (PI / 180.0)) * rx) + cx;
   *out_endy = -(cos((th0 + thd) * (PI / 180.0)) * ry) + cy;   
   
   *out_large_arc_flag = thd > 180;
   *out_sweep_flag = 1;
}/*CGCoordSpace::arc_endpoints*/

void CGCoordSpace::arc_position(double curx, double cury, double rx, double ry,
                                double x_axis_rotation, int large_arc_flag, int sweep_flag, double x, double y,
                                double *out_cx, double *out_cy, double *out_rx, double *out_ry,
                                double *out_th0, double *out_thd) {
   double sin_th, cos_th;
   double a00, a01, a10, a11;
   double dotp, mag;
   double x0, y0, x1, y1, xc, yc;
   double d;

   sin_th = sin (x_axis_rotation * (PI / 180.0));
   cos_th = cos (x_axis_rotation * (PI / 180.0));
   a00 = (curx - x) / 2;
   a01 = (cury - y) / 2;
   x1 = (a00 * cos_th) + (a01 * sin_th);
   y1 = (a00 * -sin_th) + (a01 * cos_th);

   /* Correct radii */
   d = (x1*x1)/(rx*rx) + (y1*y1)/(ry*ry);
   if (d > 1) {
      rx = sqrt(d) * rx;
      ry = sqrt(d) * ry;
   }
   /* Center calculation */
   d = ((rx*rx)*(ry*ry)) - ((rx*rx)*(y1*y1)) - ((ry*ry)*(x1*x1));
   d /= ((rx*rx)*(y1*y1)) + ((ry*ry)*(x1*x1));
   d = d < 0 ? 0 : d;  /*>>>kludge*/
   d = sqrt(d);
   if (sweep_flag == large_arc_flag) {
      d = -d;
   }
   x0 = ((rx*y1)/ry) * d;
   y0 = -((ry*x1)/rx) * d;

   xc = (cos_th * x0) + (-sin_th * y0);
   yc = (sin_th * x0) + (cos_th * y0);
   xc += (curx + x) / 2;
   yc += (cury + y) / 2;
   *out_cx = xc;
   *out_cy = yc;

   /* Start and end angle calculation */
   a00 = 1;
   a01 = 0;
   a10 = (x1-x0)/rx;
   a11 = (y1-y0)/ry;
   dotp = a00*a10 + a01*a11;
   mag = sqrt(a00*a00 + a01*a01) * sqrt(a10*a10 + a11*a11);
   d = dotp/mag > -1 ? dotp/mag : -1;
   *out_th0 = acos(d) / (PI / 180);
   if ((a00*a11 - a01*a10) < 0) {
      *out_th0 = -*out_th0;
   }
   a00 = (x1-x0)/rx;
   a01 = (y1-y0)/ry;
   a10 = (-x1-x0)/rx;
   a11 = (-y1-y0)/ry;
   dotp = a00*a10 + a01*a11;
   mag = sqrt(a00*a00 + a01*a01) * sqrt(a10*a10 + a11*a11);

   d = dotp/mag > -1 ? dotp/mag : -1;
   *out_thd = acos(d) / (PI / 180);
   if ((a00*a11 - a01*a10) < 0) {
      *out_thd = -*out_thd;
   }
   if (!sweep_flag && *out_thd > 0) {
      *out_thd -= 360;
   }
   if (sweep_flag && *out_thd < 0) {
      *out_thd += 360;
   }

  *out_rx = rx;
  *out_ry = ry;
}/*CGCoordSpace::arc_position*/

void CGCoordSpace::extent_reset(TRect *rect) {
   /*>>>indication in 'rect' that non valid data is contained?*/
   rect->point[0].x = 10000;
   rect->point[0].y = 10000;
   rect->point[1].x = -10000;
   rect->point[1].y = -10000;
}/*CGCoordSpace::extent_reset*/

bool CGCoordSpace::extent_empty(TRect *rect) {
   return (rect->point[0].x == 10000 &&
            rect->point[0].y == 10000 &&
            rect->point[1].x == -10000 &&
            rect->point[1].y == -10000);
}/*CGCoordSpace::extent_empty*/

void CGCoordSpace::extent_add(TRect *rect, int count, TPoint *point) {
   int i;

   for (i = 0; i < count; i++) {
      if (point[i].x < rect->point[0].x) {
         rect->point[0].x = point[i].x;
      }
      if (point[i].x > rect->point[1].x) {
         rect->point[1].x = point[i].x;
      }
      if (point[i].y < rect->point[0].y) {
         rect->point[0].y = point[i].y;
      }
      if (point[i].y > rect->point[1].y) {
         rect->point[1].y = point[i].y;
      }
   }
}/*CGCoordSpace::extent_add*/

void CGCoordSpace::extent_extend(TRect *rect, TCoord extent) {
   rect->point[0].x -= extent;
   rect->point[0].y -= extent;
   rect->point[1].x += extent;
   rect->point[1].y += extent;
}/*CGCoordSpace::extent_extend*/

bool CGCoordSpace::area_inside_rect(ARRAY<TPoint> *area, TRect *rect) {
   if (ARRAY(area).count() == 0 || CGCoordSpace(this).extent_empty(rect)) {
      return FALSE;
   }
   else {
      /* Assume a rectangle area >>>enforce this */
      if (rect->point[0].x <= ARRAY(area).data()[0].x &&
         rect->point[1].x >= ARRAY(area).data()[2].x &&
         rect->point[0].y <= ARRAY(area).data()[0].y &&
         rect->point[1].y >= ARRAY(area).data()[2].y) {
         return TRUE;
      }
   }
   return FALSE;
}/*CGCoordSpace::area_inside_rect*/

bool CGCoordSpace::rect_inside_area(TRect *rect, ARRAY<TPoint> *area) {
   if (ARRAY(area).count() == 0 || CGCoordSpace(this).extent_empty(rect)) {
      return FALSE;
   }
   else {
      /* Assume a rectangle area >>>enforce this */
      if (rect->point[0].x >= ARRAY(area).data()[0].x &&
         rect->point[1].x <= ARRAY(area).data()[2].x &&
         rect->point[0].y >= ARRAY(area).data()[0].y &&
         rect->point[1].y <= ARRAY(area).data()[2].y) {
         return TRUE;
      }
   }
   return FALSE;
}/*CGCoordSpace::rect_inside_area*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/