/*!\file
 \brief Keystone source file

 Keystone Pre Alpha Release

 by Jacob Lister - 2002.

 contact j_lister@paradise.net.nz

 for use by Abbey Systems Wellington internally only

 DO NOT DISTRIBUTE
*/

#define ENV_DIRECTORY    "KEYSTONE_DIR"
#define ENV_TARGET       "KEYSTONE_TARGET"
#define CONFIG_FILE_NAME "keytools.cfg"

void CONFIG_new(void);
void CONFIG_delete(void);
char *CONFIG_make_filename(const char *path, const char *filename);
int  CONFIG_read_file(const char *filename);
char *CONFIG_key_get(const char *keyname);

