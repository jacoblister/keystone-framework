/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../graphics/graphics.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGGridEditCell : CObjPersistent {
 private:
   int index;

   void new(void);
   void delete(void);

   virtual void rebind(void);
   virtual void update(void);
 public:
   void CGGridEditCell(int index);

   ATTRIBUTE<CString value>;
   ATTRIBUTE<bool edit>;
   ATTRIBUTE<TGColour colour>;
   ATTRIBUTE<TGColour bg_colour>;
};

class CGGridEditCellBound : CGGridEditCell {
 private:
   void new(void);
   void delete(void);

   virtual void rebind(void);
   virtual void update(void);
 public:
   void CGGridEditCellBound(int index);

   ATTRIBUTE<CXPath binding>;
};

ARRAY:typedef<CGGridEditCell *>;

class CGGridEditRow : CObjPersistent {
 private:
   bool deleted;
   int index;
   int sort_index;
   unsigned long long rowid;

   void new(void);
   void delete(void);
   virtual void rebind(void);

   ARRAY<CGGridEditCell *> cell;
 public:
   void CGGridEditRow(int index);
};

class CGGridEditRowBound : CGGridEditRow {
 private:
   void new(void);
   void delete(void);
   virtual void rebind(void);
 public:
   ALIAS<"xul:gridrow">;
   ATTRIBUTE<CXPath path>;

   void CGGridEditRowBound(int index);
};

ARRAY:typedef<CGGridEditRow>;
ATTRIBUTE:typedef<CGGridEditRow *>;
ARRAY:typedef<CGGridEditRow *>;

ARRAY:typedef<CGGridEditRowBound>;
ARRAY:typedef<CGGridEditRowBound *>;

ENUM:typedef<EGGridEditSortOrder> {
   {ascending},
   {descending},
};

class CGGridEditRows : CObjPersistent {
 private:
   void new(void);
   void delete(void);
   void rebind(void);
   void sort(void);

   ARRAY<CGGridEditRow *> row_index;
   ARRAY<CGGridEditRow *> index;
 public:
   ALIAS<"xul:gridrows">;
   ATTRIBUTE<int count>;
   ATTRIBUTE<int height>;
   ATTRIBUTE<bool headings> {
      this->headings = data ? *data : TRUE;
   };

   void CGGridEditRows(void);
};

class CGGridEditColumn : CObjPersistent {
 private:
  void new(void);
  void delete(void);
 public:
   void CGGridEditColumn(void);

   ALIAS<"xul:gridcolumn">;
   ATTRIBUTE<int width>;
   ATTRIBUTE<CString heading>;
   ATTRIBUTE<CXPath binding>;
   ATTRIBUTE<bool editable> {
      this->editable = data ? *data : TRUE;
   };
};

class CGGridEditColumns : CObjPersistent {
 private:
  void new(void);
  void delete(void);
  int allocated_width;
 public:
   void CGGridEditColumns(void);

   ALIAS<"xul:gridcolumns">;
   ATTRIBUTE<bool headings> {
      if (data) {
         this->headings = *data;
      }
      else {
         this->headings = TRUE;
      }
   };
   ATTRIBUTE<CXPath path>;
   ATTRIBUTE<bool sortable> {
      this->sortable = data ? *data : TRUE;
   };
   ATTRIBUTE<int sort_index, "sortIndex"> {
      this->sort_index = data ? *data : -1;
   };
   ATTRIBUTE<EGGridEditSortOrder sort_direction, "sortDirection">;
};

class CGGridEdit;

class CGGridEditLayout : CGLayout {
 private:
   CGXULElement *cell_edit;

   void set_font(CGCanvas *canvas);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   void navigate_event(CGGridEditRow *row, CGGridEditColumn *column, CEvent *event);
   virtual bool event(CEvent *event);
   void new(void);
   void delete(void);
   void cell_extent(int row_index, int column_index, TRect *result);
 public:
   void CGGridEditLayout(CGGridEdit *grid_edit);

   void cell_edit(CGGridEditCell *cell);
   void cell_edit_update(void);

   int rows_visible;
   ATTRIBUTE<int curpos_x> {
      if (data) {
         this->curpos_x = *data;
         CGGridEditLayout(this).cell_edit_update();
      }
   };
   ATTRIBUTE<int curpos_y> {
      if (data) {
         this->curpos_y = *data;
         CGGridEditLayout(this).cell_edit_update();
      }
   };
};

ENUM:typedef<EGGridEditState> {
   {select},
   {edit},
};

ENUM:typedef<EGGridEditCellEdit> {
   {allocate},
   {release},
   {del},
};

class CGGridEdit : CGScrolledArea {
 private:
   CObjServer *obj_server;
   CObjPersistent *data_source;
   CGGridEditLayout layout;
   CSelection selection;
   EGGridEditState state;
   bool notify_selection;

   virtual bool keyboard_input(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_all_update(bool changing);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);

   virtual void notify_cell_edit(EGGridEditCellEdit action, CGGridEditCell *cell, CGXULElement **edit_control);
   int column_max_width(CGGridEditColumn *column);

   void new(void);
   void delete(void);
 public:
   ALIAS<"xul:gridedit">;
   ELEMENT:OBJECT<CGGridEditColumns columns, "xul:gridcolumns">;
   ELEMENT:OBJECT<CGGridEditRows rows, "xul:gridrows">;
   void CGGridEdit(CObjServer *obj_server, CObjPersistent *data_source);
   void clear(void);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#define GRID_SCROLL_SIZE 18
#define GRID_ROW_HEADING_WIDTH 40

void CGGridEditCell::new(void) {
   new(&this->value).CString(NULL);

   this->colour = GCOL_BLACK;
   this->bg_colour = GCOL_WHITE;
}/*CGGridEditCell::new*/

void CGGridEditCell::CGGridEditCell(int index) {
   this->index = index;
}/*CGGridEditCell::CGGridEditCell*/

void CGGridEditCell::delete(void) {
   delete(&this->value);
}/*CGGridEditCell::delete*/

void CGGridEditCell::rebind(void) {}
void CGGridEditCell::update(void) {}

void CGGridEditCellBound::new(void) {
   class:base.new();

   new(&this->binding).CXPath(NULL, NULL);
}/*CGGridEditCellBound::new*/

void CGGridEditCellBound::CGGridEditCellBound(int index) {
   CGGridEditCell(this).CGGridEditCell(index);
}/*CGGridEditCellBound::CGGridEditCellBound*/

void CGGridEditCellBound::delete(void) {
   delete(&this->binding);

   class:base.delete();
}/*CGGridEditCellBound::delete*/

void CGGridEditCellBound::rebind(void) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent_find(&class(CGGridEdit)));
   CGGridEditColumn *column = CGGridEditColumn(CObject(&grid->columns).child_n(CGGridEditCell(this)->index));

   CXPath(&this->binding).path_set(CString(&column->binding.path).string());

   CGGridEditCellBound(this).update();
}/*CGGridEditCellBound::rebind*/

void CGGridEditCellBound::update(void) {
/*   if (this->binding.object.object && this->binding.object.attr.attribute && this->binding.object.attr.attribute->type == PT_AttributeEnum &&
      CObjPersistent(this->binding.object.object).attribute_default_get(this->binding.object.attr.attribute)) {
   }
   else */if (this->binding.object.object) {
      CXPath(&this->binding).get_string(&CGGridEditCell(this)->value);
      if (this->binding.object.object && this->binding.object.attr.attribute) {
         CGGridEditCell(this)->colour = CObjPersistent(this->binding.object.object).attribute_default_get(this->binding.object.attr.attribute)
            ? GCOL_RGB(0xA0, 0xA0, 0xA0) : GCOL_BLACK;
      }
   }
}/*CGGridEditCellBound::update*/

void CGGridEditColumns::new(void) {
   new(&this->path).CXPath(NULL, NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditColumns,headings>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditColumns,sortable>, TRUE);
   class:base.new();
}/*CGGridEditColumns::new*/

void CGGridEditColumns::CGGridEditColumns(void) {
}/*CGGridEditColumns::CGGridEditColumns*/

void CGGridEditColumns::delete(void) {
   class:base.delete();
   delete(&this->path);
}/*CGGridEditColumns::delete*/

void CGGridEditColumn::new(void) {
   new(&this->binding).CXPath(NULL, NULL);
   new(&this->heading).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditColumn,editable>, TRUE);
   class:base.new();
}/*CGGridEditColumn::new*/

void CGGridEditColumn::CGGridEditColumn(void) {
}/*CGGridEditColumn::CGGridEditColumn*/

void CGGridEditColumn::delete(void) {
   class:base.delete();
   delete(&this->heading);
   delete(&this->binding);
}/*CGGridEditColumn::delete*/

void CGGridEditRow::new(void) {
   ARRAY(&this->cell).new();
}/*CGGridEditRow::new*/

void CGGridEditRow::CGGridEditRow(int index) {
   this->index = index;
   this->sort_index = index;
}/*CGGridEditRow::CGGridEditRow*/

void CGGridEditRow::delete(void) {
   int i;
   CGGridEditCell *cell;

   for (i = 0; i < ARRAY(&this->cell).count(); i++) {
      cell = ARRAY(&this->cell).data()[i];
      delete(cell);
   }

   ARRAY(&this->cell).delete();
}/*CGGridEditRow::delete*/

void CGGridEditRow::rebind(void) {
   int i;
   CGGridEditCell *cell;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent_find(&class(CGGridEdit)));
   /*>>>delete existing cells*/

   ARRAY(&CGGridEditRow(this)->cell).used_set(CObject(&grid->columns).child_count());
   for (i = 0; i < CObject(&grid->columns).child_count(); i++) {
      cell = new.CGGridEditCell(i);
      CObject(cell).parent_set(CObject(this));
      ARRAY(&CGGridEditRow(this)->cell).data()[i] = CGGridEditCell(cell);
   }
}/*CGGridEditRow::rebind*/

void CGGridEditRowBound::new(void) {
   class:base.new();

   new(&this->path).CXPath(NULL, NULL);
}/*CGGridEditRowBound::new*/

void CGGridEditRowBound::CGGridEditRowBound(int index) {
   CGGridEditRow(this).CGGridEditRow(index);
}/*CGGridEditRowBound::CGGridEditRowBound*/

void CGGridEditRowBound::delete(void) {
   delete(&this->path);

   class:base.delete();
}/*CGGridEditRowBound::delete*/

void CGGridEditRowBound::rebind(void) {
   int i;
   CGGridEditCellBound *cell;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent_find(&class(CGGridEdit)));
   /*>>>delete existing cells*/

   ARRAY(&CGGridEditRow(this)->cell).used_set(CObject(&grid->columns).child_count());
   for (i = 0; i < CObject(&grid->columns).child_count(); i++) {
      cell = new.CGGridEditCellBound(i);
      CObject(cell).parent_set(CObject(this));
      ARRAY(&CGGridEditRow(this)->cell).data()[i] = CGGridEditCell(cell);

      CXPath(&cell->binding).initial_set(&this->path.object);
      CGGridEditCell(cell).rebind();
   }
}/*CGGridEditRowBound::rebind*/

void CGGridEditRows::new(void) {
   ARRAY(&this->row_index).new();
   ARRAY(&this->index).new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditRows,headings>, TRUE);
}/*CGGridEditRows::new*/

void CGGridEditRows::CGGridEditRows(void) {
}/*CGGridEditRows::CGGridEditRows*/

void CGGridEditRows::delete(void) {
   ARRAY(&this->index).delete();
   ARRAY(&this->row_index).delete();
}/*CGGridEditRows::delete*/

void CGGridEditRows::rebind(void) {
   int i;
   CGGridEditRow *row;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());

   /* delete existing contents */
   for (i = 0; i < ARRAY(&this->row_index).count(); i++) {
      row = ARRAY(&this->row_index).data()[i];
      delete(row);
   }

   ARRAY(&this->row_index).used_set(this->count);
   for (i = 0; i < this->count; i++) {
      row = (CGGridEditRow *) new.CGGridEditRowBound(i);
      ARRAY(&this->row_index).data()[i] = row;
      CObject(row).parent_set(CObject(this));

      CXPath(&CGGridEditRowBound(row)->path).initial_set(&grid->columns.path.initial);
      CXPath(&CGGridEditRowBound(row)->path).index_set(i);
      CXPath(&CGGridEditRowBound(row)->path).path_set(CString(&grid->columns.path.path).string());
      CGGridEditRow(row).rebind();
   }

   CGGridEditRows(this).sort();
}/*CGGridEditRows::rebind*/


bool ATTRIBUTE:convert<CGGridEditRow *>(CObjPersistent *object,
                                        const TAttributeType *dest_type, const TAttributeType *src_type,
                                        int dest_index, int src_index,
                                        void *dest, const void *src) {
   CGGridEditRow *row_src, *row_dest;
   CGGridEditCell *cell_src, *cell_dest;
   CGGridEdit *grid;
   int *result;

   if (dest_type == &ATTRIBUTE:type<CGGridEditRow *> && src_type == NULL) {
      memset(dest, 0, sizeof(CGGridEditRow *));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CGGridEditRow *> && src_type == &ATTRIBUTE:type<CGGridEditRow *>) {
      *((CGGridEditRow **)dest) = *((CGGridEditRow **)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CGGridEditRow *>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<op_compare> && src_type == &ATTRIBUTE:type<CGGridEditRow *>) {
      result = (int *)object;
      row_src  = *(CGGridEditRow **)src;
      row_dest = *(CGGridEditRow **)dest;
      grid = CGGridEdit(CObject(row_src).parent_find(&class(CGGridEdit)));
      if (grid->columns.sort_index == -1) {
         if (row_dest->index == row_src->index) {
            *result = 0;
         }
         else if (row_dest->index < row_src->index) {
            *result = -1;
         }
         else {
            *result = 1;
         }
      }
      else {
         /* just string compare for now >>>use native type compare */
         cell_dest = ARRAY(&row_dest->cell).data()[grid->columns.sort_index];
         cell_src = ARRAY(&row_src->cell).data()[grid->columns.sort_index];

         *result = CString(&cell_dest->value).strcmp(CString(&cell_src->value).string());

      }
      return TRUE;
   }

   return FALSE;
}

void CGGridEditRows::sort(void) {
   CGGridEditRow *row;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   int i;

   ARRAY(&this->index).used_set(0);
   for (i = 0; i < ARRAY(&this->row_index).count(); i++) {
      row = ARRAY(&this->row_index).data()[i];
      if (!row->deleted) {
         ARRAY(&this->index).item_add(row);
      }
   }

   ARRAY(&this->index).sort_type(&ATTRIBUTE:type<CGGridEditRow *>, grid->columns.sort_direction);

   for (i = 0; i < ARRAY(&this->index).count(); i++) {
      row = ARRAY(&this->index).data()[i];
      row->sort_index = i;
   }
}/*CGGridEditRows::sort*/

void CGGridEditLayout::new(void) {
   class:base.new();
}/*CGGridEditLayout::new*/

void CGGridEditLayout::CGGridEditLayout(CGGridEdit *grid_edit) {
   CGLayout(this).CGLayout(0, 0, grid_edit->obj_server, grid_edit->data_source);
}/*CGGridEditLayout::CGGridEditLayout*/

void CGGridEditLayout::delete(void) {
   class:base.delete();
}/*CGGridEditLayout::delete*/

void CGGridEditLayout::set_font(CGCanvas *canvas) {
   CGFontFamily font_family;

   new(&font_family).CGFontFamily();
   CString(&font_family).set("Arial");
   CGCanvas(canvas).NATIVE_font_set(&font_family, 12, EGFontStyle.normal, EGFontWeight.normal, EGTextDecoration.none);
   CGCanvas(canvas).NATIVE_text_align_set(EGTextAnchor.start, EGTextBaseline.central);
   delete(&font_family);
}/*CGGridEditLayout::set_font*/

void CGGridEditLayout::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditColumn *column;
   CGGridEditRow *row;
   CGGridEditCell *cell;
   TPoint sort_point[3];
   int row_index, row_start, row_end;
   int column_index, column_start, column_end, x, y;
   int column_width_total = 0, row_height_total;
   TRect cell_extent;
   char str[10];

   if (!extent) {
      extent = &CGObject(canvas)->extent;
   }

   CGCanvas(canvas).stroke_style_set(1, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
   CGCanvas(canvas).draw_rectangle((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    TRUE, 0, 0, CGXULElement(this)->width, CGXULElement(this)->height);

   row_start = this->curpos_y;
   row_end = this->curpos_y + this->rows_visible + 1;
   if (row_end > grid->rows.count - 1) {
      row_end = grid->rows.count - 1;
   }
   column_start = this->curpos_x;
   column_end = CObject(&grid->columns).child_count() - 1;

   x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
   column = CGGridEditColumn(CObject(&grid->columns).child_n(column_start));
   for (column_index = column_start; column_index <= column_end; column_index++) {
      x += column->width;
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 x, extent->point[0].y, x, /*extent->point[1].y*/(grid->rows.count + (grid->columns.headings ? 1 : 0)) * grid->rows.height);
      column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
   }

   CGGridEditLayout(this).set_font(canvas);

   y = grid->columns.headings ? grid->rows.height : 0;
   for (row_index = row_start; row_index <= row_end; row_index++) {
      row = ARRAY(&grid->rows.index).data()[row_index];
      x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
      column = CGGridEditColumn(CObject(&grid->columns).child_n(column_start));
      for (column_index = column_start; column_index <= column_end; column_index++) {
         cell = ARRAY(&row->cell).data()[column_index];
         CGGridEditCell(cell).update();         /* update cell contents >>> not efficient, done every redraw in this case */
         CGCanvas(canvas).draw_rectangle(cell->bg_colour,
                                         TRUE, x + 1, y + 1, x + column->width - 1, y + grid->rows.height);
         CGCanvas(canvas).draw_text(cell->colour, x + 3, y + grid->rows.height / 2 + 1, CString(&cell->value).string());
         x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
      }
      y += grid->rows.height;
      column_width_total = x;
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 extent->point[0].x, y, /*extent->point[1].x*/x, y);
   }
   row_height_total = y;

   /* Draw Selection */
   if (CSelection(&grid->selection).count() && !this->cell_edit) {
      CGCanvas(canvas).stroke_style_set(3, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
      if (CObjClass_is_derived(&class(CGGridEditCell), CObject((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object).obj_class())) {
         cell = CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object);
         row = CGGridEditRow(CObject(cell).parent());

         CGGridEditLayout(this).cell_extent(row->sort_index, cell->index, &cell_extent);
         CGCanvas(canvas).draw_rectangle(GCOL_TYPE_NAMED | EGColourNamed.black, FALSE,
                                         cell_extent.point[0].x, cell_extent.point[0].y,
                                         cell_extent.point[1].x, cell_extent.point[1].y);
      }
      if (CObjClass_is_derived(&class(CGGridEditRow), CObject((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object).obj_class())) {
         row = CGGridEditRow((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object);

         CGGridEditLayout(this).cell_extent(row->sort_index, -1, &cell_extent);
         CGCanvas(canvas).draw_rectangle(GCOL_TYPE_NAMED | EGColourNamed.black, FALSE,
                                         cell_extent.point[0].x, cell_extent.point[0].y,
                                         cell_extent.point[1].x, cell_extent.point[1].y);
      }
   }

   /* Draw Headings */
   CGCanvas(canvas).NATIVE_text_align_set(EGTextAnchor.middle, EGTextBaseline.central);
   CGCanvas(canvas).stroke_style_set(1, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
   if (grid->columns.headings) {
      CGCanvas(canvas).draw_rectangle((GCOL_TYPE_NAMED | EGColourNamed.lavender), TRUE,
                                      extent->point[0].x, 0, column_width_total, grid->rows.height);
      x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
      column = CGGridEditColumn(CObject(&grid->columns).child_n(column_start));
      for (column_index = column_start; column_index <= column_end; column_index++) {
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    x + 1, 0, x + column->width - 1, 0);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    x + 1, 0, x + 1, grid->rows.height);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                    x + column->width, 0, x + column->width, grid->rows.height);
         CGCanvas(canvas).draw_text(GCOL_TYPE_NAMED | EGColourNamed.black, x + column->width / 2, grid->rows.height / 2 + 1, CString(&column->heading).string());

         if (column_index == grid->columns.sort_index) {
            sort_point[0].x = x + 10;
            sort_point[0].y = grid->rows.height / 2 + (grid->columns.sort_direction == EGGridEditSortOrder.ascending ? -4 : 4);
            sort_point[1].x = x + 14;
            sort_point[1].y = grid->rows.height / 2 + (grid->columns.sort_direction == EGGridEditSortOrder.ascending ? 4 : -4);
            sort_point[2].x = x + 6;
            sort_point[2].y = grid->rows.height / 2 + (grid->columns.sort_direction == EGGridEditSortOrder.ascending ? 4 : -4);
            CGCanvas(canvas).draw_polygon(GCOL_TYPE_NAMED | EGColourNamed.lightgrey, TRUE, 3, sort_point, TRUE);
         }

         x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
      }
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 extent->point[0].x, grid->rows.height, /*extent->point[1].x*/x, grid->rows.height);
   }

   if (grid->rows.headings) {
      y = grid->columns.headings ? grid->rows.height : 0;

      CGCanvas(canvas).draw_rectangle((GCOL_TYPE_NAMED | EGColourNamed.lavender), TRUE,
                                      extent->point[0].x, y, GRID_ROW_HEADING_WIDTH, row_height_total);
      for (row_index = row_start; row_index <= row_end; row_index++) {
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    0, y + 1, GRID_ROW_HEADING_WIDTH - 1, y + 1);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    0, y + 1, 0, y + grid->rows.height - 1);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                    0, y + grid->rows.height, GRID_ROW_HEADING_WIDTH, y + grid->rows.height);
         sprintf(str, "%d", (ARRAY(&grid->rows.index).data()[row_index])->index);
         CGCanvas(canvas).draw_text(GCOL_TYPE_NAMED | EGColourNamed.black, GRID_ROW_HEADING_WIDTH / 2, y + (grid->rows.height / 2) + 1,
                                    str);
         y += grid->rows.height;
      }
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 GRID_ROW_HEADING_WIDTH, extent->point[0].y, GRID_ROW_HEADING_WIDTH, y);
   }
}/*CGGridEditLayout::draw*/

void CGGridEditLayout::navigate_event(CGGridEditRow *row, CGGridEditColumn *column, CEvent *event) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditCell *cell = NULL;
   CSelection *selection;
   CObjPersistent *object;
   bool key_enter_event;

   if (!row) {
      return;
   }
   
   if (column) {
      cell = ARRAY(&row->cell).data()[CObject(CObject(column).parent()).child_index(CObject(column))];
      object = CObjPersistent(cell);
   }
   else {
      object = CObjPersistent(row);
   }

   key_enter_event =
      event && CObject(event).obj_class() == &class(CEventKey) &&
      !this->cell_edit &&
      (CEventKey(event)->key == EEventKey.Enter || CEventKey(event)->key == EEventKey.Ascii);

   if (event && ((CObject(event).obj_class() == &class(CEventPointer) && CEventPointer(event)->type == EEventPointer.LeftDClick) ||
       key_enter_event)) {
      if (column && CSelection(&grid->selection).count()) {
         if (cell == CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object) &&
             column->editable) {
            CGGridEditLayout(this).cell_edit(NULL);
            CGGridEditLayout(this).cell_edit(cell);

            if (CObject(event).obj_class() == &class(CEventKey) && CEventKey(event)->key != EEventKey.Enter) {
               CGXULElement(this->cell_edit).repeat_event(event);
            }
         }
      }
   }
   else if (event && CObject(event).obj_class() == &class(CEventKey) &&
      this->cell_edit && CEventKey(event)->key == EEventKey.Escape) {
      /*>>>escape should also cancel the current changes */
      CGGridEditLayout(this).cell_edit(NULL);
   }
   else {
      if (row && (!CSelection(&grid->selection).count() ||
                   object != (&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object)) {
         CGGridEditLayout(this).cell_edit(NULL);
         CSelection(&grid->selection).clear();
         CSelection(&grid->selection).add(object, NULL);
         CGCanvas(this).queue_draw(NULL); /*>>> can be much more efficent */

         /*>>>use selection update method in CObjServer*/
         if (CObject(row).obj_class() == &class(CGGridEditRowBound)) {
            object = CGGridEditRowBound(row)->path.initial.object;
            while (object) {
               if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
                  break;
               }
               object = CObjPersistent(CObject(object).parent());
            }
         }

         selection = CObjServer(grid->obj_server).selection_get();
         if (grid->notify_selection && selection && object) {
            CObjServer(grid->obj_server).root_selection_update();
            CSelection(selection).clear();
            CSelection(selection).add(object, NULL);
            CObjServer(grid->obj_server).root_selection_update_end();
         }
      }
   }
}/*CGGridEditLayout::navigate_event*/

bool CGGridEditLayout::event(CEvent *event) {
   int row_index = -1, column_index = -1, x;
   CGGridEditRow *row = NULL;
   CGGridEditColumn *column = NULL;
   CGGridEditCell *cell = NULL;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CObjPersistent *object = NULL;
   int newpos = this->curpos_y;

   if (CObject(event).obj_class() == &class(CEventKey) && CEventKey(event)->type == EEventKeyType.down) {
      /*>>>keyboard navigation should scroll layout as well if needed */
      if (CSelection(&grid->selection).count()) {
         object = (&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object;
      }
      if (object && CObjClass_is_derived(&class(CGGridEditRow), CObject(object).obj_class())) {
         row = CGGridEditRow(object);
      }
      if (object && CObjClass_is_derived(&class(CGGridEditCell), CObject(object).obj_class())) {
         cell = CGGridEditCell(object);
         row = CGGridEditRow(CObject(cell).parent());
         column_index = cell->index;
         column = CGGridEditColumn(CObject(&grid->columns).child_n(cell->index));
      }

      if (row) {
         row_index = row->sort_index;
      }
      else {
         row_index = 0;
      }

      if (!column) {
         column_index = 0;
      }

      switch (CEventKey(event)->key) {
      case EEventKey.Delete:
         if (row && !column && CObject(row).obj_class() == &class(CGGridEditRowBound)) {
            CSelection(&grid->selection).clear();
            if (CGGridEditRowBound(row)->path.object.object) {
               delete(CGGridEditRowBound(row)->path.object.object);
            }
            row->deleted = TRUE;
            grid->rows.count--;
            CGGridEditRows(&grid->rows).sort();
            CGCanvas(this).queue_draw(NULL); /*>>> can be much more efficent */
         }
         break;
      case EEventKey.Up:
         if (row_index > 0) {
            row_index--;
            row = ARRAY(&grid->rows.index).data()[row_index];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
            if (row_index < this->curpos_y) {
               newpos = row_index;
            }
         }
         break;
      case EEventKey.Down:
      key_down:
         if (row_index < ARRAY(&grid->rows.index).count() - 1) {
            row_index++;
            row = ARRAY(&grid->rows.index).data()[row_index];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
            if (row_index >= this->curpos_y + this->rows_visible) {
               newpos = row_index - this->rows_visible + 1;
            }
         }
         break;
      case EEventKey.Left:
         if (column_index > 0) {
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index -1));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.Right:
         if (column_index < CObject(&grid->columns).child_count() - 1) {
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index + 1));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.Enter:
         if (this->cell_edit)
            goto key_down;
         /*else fallthrough*/
      case EEventKey.Escape:
      case EEventKey.Ascii:
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      case EEventKey.Tab:
         if (CEventKey(event)->modifier) {
            if (column_index > 0) {
               column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index - 1));
            }
            else {
               column = CGGridEditColumn(CObject(&grid->columns).child_last());
                  if (row_index > 0) {
                     row = ARRAY(&grid->rows.index).data()[row_index - 1];
               }
               else {
                  break;
               }
            }
         }
         else {
            if (column_index < CObject(&grid->columns).child_count() - 1) {
               column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index + 1));
            }
            else {
               column = CGGridEditColumn(CObject(&grid->columns).child_first());
               if (row_index < ARRAY(&grid->rows.index).count() - 1) {
                  row = ARRAY(&grid->rows.index).data()[row_index + 1];
               }
               else {
                  break;
               }
            }
         }
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      case EEventKey.PgUp:
         if (row_index > this->rows_visible) {
            newpos = this->curpos_y - this->rows_visible;
            row = ARRAY(&grid->rows.index).data()[row_index - this->rows_visible];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.PgDn:
         if (row_index < ARRAY(&grid->rows.index).count() - this->rows_visible) {
            newpos = this->curpos_y + this->rows_visible;
            row = ARRAY(&grid->rows.index).data()[row_index + this->rows_visible];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.Home:
         newpos = 0;
         row = ARRAY(&grid->rows.index).data()[0];
         column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      case EEventKey.End:
         newpos = CGScrollBar(&CGScrolledArea(grid)->scroll_vert)->maxpos;
         row = ARRAY(&grid->rows.index).data()[ARRAY(&grid->rows.index).count() - 1];
         column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      default:
         break;
      }
   }
   if (CObject(event).obj_class() == &class(CEventPointer)) {
      switch (CEventPointer(event)->type) {
      case EEventPointer.ScrollDown:
         newpos = this->curpos_y + 3;
         break;
      case EEventPointer.ScrollUp:
         newpos = this->curpos_y - 3;
         break;
      case EEventPointer.LeftDown:
      case EEventPointer.LeftDClick:
         CGXULElement(this).NATIVE_focus_in();
         CGLayout(this).key_gselection_set(CGObject(this));

         row_index = (int)(CEventPointer(event)->position.y / grid->rows.height) + this->curpos_y - grid->columns.headings;
         x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
         column = CGGridEditColumn(CObject(&grid->columns).child_n(this->curpos_x));
         column_index = this->curpos_x;
         while (column) {
            if (CEventPointer(event)->position.x >= x && CEventPointer(event)->position.x < x + column->width) {
               break;
            }
            column_index++;
            x += column->width;
            column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
         }

         if (grid->columns.headings && (int)(CEventPointer(event)->position.y / grid->rows.height) == 0 &&
             grid->columns.sortable) {
            if (!column) {
               column_index = -1;
            }
            if (column_index == grid->columns.sort_index) {
               grid->columns.sort_direction = !grid->columns.sort_direction;
            }
            else {
               grid->columns.sort_index = column_index;
               grid->columns.sort_direction = EGGridEditSortOrder.ascending;
            }
            CGGridEditRows(&grid->rows).sort();
            CGCanvas(this).queue_draw(NULL); /*>>> can be much more efficent */
         }
         else if (row_index < grid->rows.count) {
            if (row_index >= 0) {
               row = ARRAY(&grid->rows.index).data()[row_index];
               CGGridEditLayout(this).navigate_event(row, column, event);
            }
         }
         break;
      default:
         break;
      }
   }

   if (newpos != this->curpos_y) {
      if (newpos > CGScrollBar(&CGScrolledArea(grid)->scroll_vert)->maxpos) {
         newpos = CGScrollBar(&CGScrolledArea(grid)->scroll_vert)->maxpos;
      }
      if (newpos < 0) {
         newpos = 0;
      }
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGGridEditLayout,curpos_y>);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGGridEditLayout,curpos_y>, newpos);
      CObjPersistent(this).attribute_update_end();
      CObjPersistent(&CGScrolledArea(grid)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
      CObjPersistent(&CGScrolledArea(grid)->scroll_vert).attribute_set_int(ATTRIBUTE<CGScrollBar,curpos>, newpos);
      CObjPersistent(&CGScrolledArea(grid)->scroll_vert).attribute_update_end();
   }

   if (CObject(event).obj_class() == &class(CEventKey)) {
      switch (CEventKey(event)->key) {
      case EEventKey.Escape:
         break;
      default:
         return TRUE;
      }
   }
   return class:base.event(event);
}/*CGGridEditLayout::event*/

void CGGridEditLayout::cell_edit(CGGridEditCell *cell) {
   TRect cell_extent;
   CGGridEditRow *row;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditCell *current_cell;
   CGWindow *window = CGWindow(CObject(this).parent_find(&class(CGWindow)));

   if (this->cell_edit) {
      current_cell = CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object);
      CGXULElement(this->cell_edit).NATIVE_focus_out();
      CGXULElement(window).NATIVE_focus_in(); /*>>>cheap*/
      CGGridEdit(grid).notify_cell_edit(EGGridEditCellEdit.release, current_cell, &this->cell_edit);

      delete(this->cell_edit);
      this->cell_edit = NULL;
      CGLayout(this).key_gselection_set(CGObject(this));
   }
   if (cell && CObject(cell).obj_class() == &class(CGGridEditCellBound)) {
      CGGridEdit(grid).notify_cell_edit(EGGridEditCellEdit.allocate, cell, NULL);

      row = CGGridEditRow(CObject(cell).parent());
      CGGridEditLayout(this).cell_extent(row->sort_index, cell->index, &cell_extent);
      if (CGGridEditCellBound(cell)->binding.object.object &&
          CGGridEditCellBound(cell)->binding.object.attr.attribute->type == PT_AttributeEnum) {
         if (CObjPersistent(CGGridEditCellBound(cell)->binding.object.object).attribute_default_get(CGGridEditCellBound(cell)->binding.object.attr.attribute)) {
            /*>>>kludge, don't allow editing of default enums */
            return;
         }
         this->cell_edit = (CGXULElement *)new.CGMenuList((int)cell_extent.point[0].x, (int)(cell_extent.point[0].y - 1),
                                                          (int)(cell_extent.point[1].x - cell_extent.point[0].x + 1),
                                                          (int)(cell_extent.point[1].y - cell_extent.point[0].y + 2));
      }
      else {
         this->cell_edit = (CGXULElement *)new.CGTextBox((int)cell_extent.point[0].x, (int)(cell_extent.point[0].y - 1),
                                                         (int)(cell_extent.point[1].x - cell_extent.point[0].x + 1),
                                                         (int)(cell_extent.point[1].y - cell_extent.point[0].y + 2), NULL);
      }
      CObject(this).child_add(CObject(this->cell_edit));
      CXPath(&CGXULElement(this->cell_edit)->binding).set(CGGridEditCellBound(cell)->binding.object.object,
                                                          CGGridEditCellBound(cell)->binding.object.attr.attribute,
                                                          CGGridEditCellBound(cell)->binding.object.attr.index);
      CGXULElement(this->cell_edit).binding_resolve();
      CGObject(this->cell_edit).show(TRUE);
      CGXULElement(this->cell_edit).NATIVE_focus_in();
   }
}/*CGGridEditLayout::cell_edit*/

void CGGridEditLayout::cell_edit_update(void) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditRow *row;
   CGGridEditCell *cell;
   TRect cell_extent;

   if (this->cell_edit) {
      cell = CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object); /*>>>should use edit XUL obejct binding */
      row = CGGridEditRow(CObject(cell).parent());
      CGGridEditLayout(this).cell_extent(row->sort_index, cell->index, &cell_extent);

      CGXULElement(this->cell_edit)->x      = cell_extent.point[0].x;
      CGXULElement(this->cell_edit)->y      = cell_extent.point[0].y - 1;
      CObjPersistent(this->cell_edit).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(this->cell_edit).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(this->cell_edit).attribute_update_end();
   }
}/*CGGridEditLayout::cell_edit_update*/

void CGGridEditLayout::cell_extent(int row_index, int column_index, TRect *result) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditColumn *column;
   int column_cur;

   result->point[0].x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
   column = CGGridEditColumn(CObject(&grid->columns).child_first());
   column_cur = 0;
   if (column_index == -1) {
      result->point[1].x = result->point[0].x;
      while (column) {
         result->point[1].x -= (column_cur < this->curpos_x) ? column->width : 0;
         result->point[1].x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
         column_cur++;
      }
   }
   else {
      while (column) {
         result->point[0].x -= (column_cur < this->curpos_x) ? column->width : 0;
         if (column_cur == column_index) {
            break;
         }
         result->point[0].x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
         column_cur++;
      }
      result->point[1].x = result->point[0].x + column->width;
   }

   result->point[0].y = (row_index - this->curpos_y + (grid->columns.headings ? 1 : 0)) * grid->rows.height;
   result->point[1].y = (row_index - this->curpos_y + (grid->columns.headings ? 2 : 1)) * grid->rows.height;
}/*CGGridEditLayout::cell_extent*/

void CGGridEdit::new(void) {
   class:base.new();

   this->notify_selection = TRUE;

   new(&this->columns).CGGridEditColumns();
   CObject(&this->columns).parent_set(CObject(this));
   new(&this->rows).CGGridEditRows();
   CObject(&this->rows).parent_set(CObject(this));
   new(&this->selection).CSelection(FALSE);

   CGContainer(this)->allow_child_repos = TRUE;
}/*CGGridEdit::new*/

void CGGridEdit::CGGridEdit(CObjServer *obj_server, CObjPersistent *data_source) {
   this->obj_server = obj_server;
   this->data_source = data_source;
}/*CGGridEdit::CGGridEdit*/

void CGGridEdit::delete(void) {
   delete(&this->selection);
   delete(&this->rows);
   delete(&this->columns);

   class:base.delete();
}/*CGGridEdit::delete*/

void CGGridEdit::clear(void) {
   CObject *child, *next;

   CSelection(&this->selection).clear();

   child = CObject(&this->columns).child_first();
   while (child) {
      next = CObject(&this->columns).child_next(child);
      delete(child);
      child = next;
   }
   this->rows.count = 0;
   CGGridEditRows(&this->rows).rebind();
}/*CGGridEdit::clear*/

bool CGGridEdit::keyboard_input(void) {
   return TRUE;
}

void CGGridEdit::notify_cell_edit(EGGridEditCellEdit action, CGGridEditCell *cell, CGXULElement **edit_control) { }

void CGGridEdit::NATIVE_allocate(CGLayout *layout) {
   CGObject(this)->native_object = (void *)-1;

//   /*>>>not the right place for this */
//   CGGridEditRows(&this->rows).rebind();

   new(&this->layout).CGGridEditLayout(this);
   CObject(&this->layout).parent_set(CObject(this));
   CGLayout(&this->layout).render_set(EGLayoutRender.buffered);
   CGXULElement(&this->layout).NATIVE_allocate(layout);
   CGObject(&this->layout).show(TRUE);

   new(&CGScrolledArea(this)->scroll_horiz).CGScrollBar(0, 0, 0, 0);
   CObject(&CGScrolledArea(this)->scroll_horiz).parent_set(CObject(this));
   CGXULElement(&CGScrolledArea(this)->scroll_horiz).NATIVE_allocate(layout);
   CGObject(&CGScrolledArea(this)->scroll_horiz).show(TRUE);
   CXPath(&CGXULElement(&CGScrolledArea(this)->scroll_horiz)->binding).set(CObjPersistent(&this->layout), ATTRIBUTE<CGGridEditLayout,curpos_x>, -1);

   new(&CGScrolledArea(this)->scroll_vert).CGScrollBar(0, 0, 0, 0);
   CGScrolledArea(this)->scroll_vert.orient = EGBoxOrientType.vertical;
   CObject(&CGScrolledArea(this)->scroll_vert).parent_set(CObject(this));
   CGXULElement(&CGScrolledArea(this)->scroll_vert).NATIVE_allocate(layout);
   CGObject(&CGScrolledArea(this)->scroll_vert).show(TRUE);
   CXPath(&CGXULElement(&CGScrolledArea(this)->scroll_vert)->binding).set(CObjPersistent(&this->layout), ATTRIBUTE<CGGridEditLayout,curpos_y>, -1);
}/*CGGridEdit::NATIVE_allocate*/

void CGGridEdit::NATIVE_release(CGLayout *layout) {
   CGObject(this)->native_object = NULL;

   delete(&CGScrolledArea(this)->scroll_horiz);
   delete(&CGScrolledArea(this)->scroll_vert);

   CGXULElement(&this->layout).NATIVE_release(layout);
   delete(&this->layout);
}/*CGGridEdit::NATIVE_release*/

int CGGridEdit::column_max_width(CGGridEditColumn *column) {
   CGGridEditRow *row;
   CGGridEditCell *cell;
   int row_index, column_index, width, max_width = 0;
   TRect text_extent;

   CGCanvas(&this->layout).NATIVE_enter(TRUE);
   CGGridEditLayout(&this->layout).set_font(CGCanvas(&this->layout));

   column_index = CObject(&this->columns).child_index(CObject(column));
   for (row_index = 0; row_index < ARRAY(&this->rows.index).count(); row_index++) {
      row = ARRAY(&this->rows.index).data()[row_index];
      cell = ARRAY(&row->cell).data()[column_index];
      CGGridEditCell(cell).update();
      CGCanvas(&this->layout).NATIVE_text_extent_get_string(0, 0, &cell->value, &text_extent);
      width = (int)(text_extent.point[1].x - text_extent.point[0].x);
      max_width = width > max_width ? width : max_width;
   }
   CGCanvas(&this->layout).NATIVE_enter(FALSE);

   max_width = max_width < 120 ? 120 : max_width + 5;

   return max_width;
}/*CGGridEdit::column_max_width*/

void CGGridEdit::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGGridEditColumn *column;
   int width;
   int columns_visible;

   if (changing)
      return;

   if (CGObject(this)->native_object) {
      columns_visible = 0;
      width = this->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
      column = CGGridEditColumn(CObject(&this->columns).child_last());
      column = column ? CGGridEditColumn(CObject(&this->columns).child_previous(CObject(column))) : NULL;
      while (column) {
         if (CObjPersistent(column).attribute_default_get(ATTRIBUTE<CGGridEditColumn,width>)) {
            column->width = CGGridEdit(this).column_max_width(column);
         }
         width += column->width;
         if (width <= CGXULElement(this)->width) {
            columns_visible++;
         }
         column = CGGridEditColumn(CObject(&this->columns).child_previous(CObject(column)));
      }
      column = CGGridEditColumn(CObject(&this->columns).child_last());
      if (column) {
         if (CObjPersistent(column).attribute_default_get(ATTRIBUTE<CGGridEditColumn,width>)) {
            column->width = (int)(CGXULElement(this)->width - width);
            if (column->width < 120) {
               column->width = 120;
            }
         }
         width += column->width;
      }
      if (width <= CGXULElement(this)->width) {
         columns_visible++;
      }
      this->columns.allocated_width = width;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->x      = CGXULElement(this)->x;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->y      = CGXULElement(this)->y + CGXULElement(this)->height - GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->width  = CGXULElement(this)->width - GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->height = GRID_SCROLL_SIZE;
      CGScrollBar(&CGScrolledArea(this)->scroll_horiz)->curpos  = 0;
      CGScrollBar(&CGScrolledArea(this)->scroll_horiz)->maxpos  = CObject(&this->columns).child_count() - columns_visible;
      CGScrollBar(&CGScrolledArea(this)->scroll_horiz)->range   = CObject(&this->columns).child_count() - 1;
      CGObject(&CGScrolledArea(this)->scroll_horiz)->visibility = columns_visible < CObject(&this->columns).child_count();
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGObject,visibility>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGScrollBar,maxpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGScrollBar,range>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update_end();

      CGXULElement(&CGScrolledArea(this)->scroll_vert)->x      = CGXULElement(this)->x + CGXULElement(this)->width - GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_vert)->y      = CGXULElement(this)->y;
      CGXULElement(&CGScrolledArea(this)->scroll_vert)->width  = GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_vert)->height = CGXULElement(this)->height - (CGObject(&CGScrolledArea(this)->scroll_horiz)->visibility ? GRID_SCROLL_SIZE : 0);
      this->layout.rows_visible = (int)(CGXULElement(this)->height / this->rows.height - (this->columns.headings ? 1 : 0));
      if (this->rows.count > this->layout.rows_visible &&
         CGScrollBar(&CGScrolledArea(this)->scroll_vert)->curpos > (this->rows.count - this->layout.rows_visible)) {
         this->layout.curpos_y = this->rows.count - this->layout.rows_visible;
         CGScrollBar(&CGScrolledArea(this)->scroll_vert)->curpos = this->layout.curpos_y;
      }
//      CGScrollBar(&CGScrolledArea(this)->scroll_vert)->curpos  = 0;
      CGScrollBar(&CGScrolledArea(this)->scroll_vert)->maxpos  = this->rows.count - this->layout.rows_visible;
      CGScrollBar(&CGScrolledArea(this)->scroll_vert)->range   = this->rows.count - 1;
      CGObject(&CGScrolledArea(this)->scroll_vert)->visibility = this->layout.rows_visible < this->rows.count;
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGObject,visibility>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,maxpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,range>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update_end();

      CGXULElement(&this->layout)->x      = CGXULElement(this)->x;
      CGXULElement(&this->layout)->y      = CGXULElement(this)->y;
      CGXULElement(&this->layout)->width  = CGXULElement(this)->width - (CGObject(&CGScrolledArea(this)->scroll_vert)->visibility ? GRID_SCROLL_SIZE : 0);
      CGXULElement(&this->layout)->height = CGXULElement(this)->height - (CGObject(&CGScrolledArea(this)->scroll_horiz)->visibility ? GRID_SCROLL_SIZE : 0);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(&this->layout).attribute_update_end();
   }

   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}/*CGGridEdit::notify_attribute_update*/

void CGGridEdit::notify_all_update(bool changing) {
   if (!changing) {
      CGGridEditRows(&this->rows).rebind();
   }
//>>>   class:base.notify_all_update(changing);
}/*CGGridEdit::notify_all_update*/

void CGGridEdit::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGLayout *layout;
   switch (linkage) {
   case EObjectLinkage.ParentSet:
      layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
      this->obj_server = CObjClient(&layout->client)->server;
      this->data_source = ARRAY(&CObjClient(&layout->client)->object_root).data()[0];
      break;
   default:
      break;
   }
}/*CGGridEdit::notify_object_linkage*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
