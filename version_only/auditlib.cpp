// auditlib.so

#include <stdio.h>
#include <link.h>

// Snippets included from:
// https://man7.org/linux/man-pages/man7/rtld-audit.7.html

/* 
   unsigned int la_version(unsigned int version);

   This is the only function that must be defined by an auditing
   library: it performs the initial handshake between the dynamic
   linker and the auditing library.  When invoking this function,
   the dynamic linker passes, in version, the highest version of the
   auditing interface that the linker supports.

   A typical implementation of this function simply returns the
   constant LAV_CURRENT, which indicates the version of <link.h>
   that was used to build the audit module.  If the dynamic linker
   does not support this version of the audit interface, it will
   refuse to activate this audit module.  If the function returns
   zero, the dynamic linker also does not activate this audit
   module.

   In order to enable backwards compatibility with older dynamic
   linkers, an audit module can examine the version argument and
   return an earlier version than LAV_CURRENT, assuming the module
   can adjust its implementation to match the requirements of the
   previous version of the audit interface.  The la_version function
   should not return the value of version without further checks
   because it could correspond to an interface that does not match
   the <link.h> definitions used to build the audit module.
*/

__attribute__((constructor))
static void init(void) {
  printf("I'm loaded from LD_AUDIT ");
}

unsigned int la_version(unsigned int version) {
  // Prepare output - top of the yaml, and first event (version)

  printf("%d", version);

  // If version == 0 the library will be ignored by the linker.
  if (version == 0) {
    return version;
  } 
  return LAV_CURRENT;
}
