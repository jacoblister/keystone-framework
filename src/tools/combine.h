#define COMBINE_MAX_INCLUDE 60
#define COMBINE_MAX_SEARCH  10
#define COMBINE_MAX_USED    2000
#define COMBINE_MAX_FILENAME_LENGTH 200

typedef struct {
   char filename[COMBINE_MAX_FILENAME_LENGTH];
   int parent_line;
   FILE *file;
} TCombineInclude;

typedef struct {
   ECCompiler ccompiler;
   bool noinclude;
   bool stripcomment;
   const char *lineprefix;	
   int include_count;
   TCombineInclude include_file[COMBINE_MAX_INCLUDE];

   FILE *outfile;
   bool incomment;

   int path_count;
   char search_path[COMBINE_MAX_SEARCH][COMBINE_MAX_FILENAME_LENGTH];

   int used_count;
   char filename_used[COMBINE_MAX_USED][COMBINE_MAX_FILENAME_LENGTH];
} TCombine;

void COMBINE_new(TCombine *this);
void COMBINE_delete(TCombine *this);
void COMBINE_include_clear(TCombine *this);
void COMBINE_include_add(TCombine *this, const char *include_path);
void COMBINE_process(TCombine *this, ECCompiler ccompiler, bool noinclude, bool stripcomment, const char *infile, const char *outfile);
