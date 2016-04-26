#include "util.h"

void setUnicode() {
#ifdef UNICODE
_setmode(_fileno(stdin), _O_WTEXT);
_setmode(_fileno(stdout), _O_WTEXT);
#endif
}
