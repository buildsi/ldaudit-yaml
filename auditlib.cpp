// auditlib.so

#include <bits/stdc++.h>
#include <sys/stat.h>
#include <string>
#include <link.h>

// Snippets included from:
// https://man7.org/linux/man-pages/man7/rtld-audit.7.html

// Check if an output file exists
inline bool exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

// Init file clears the file to prepare for new output
static void initFile(char * filename) {
    std::fstream file;
    file.open(filename, std::ios::out);
    if(!file) {
      printf("Error in creating output file.\n");
      exit (EXIT_FAILURE);
    }
    file.close();
}  

// Shared function for printing output to file or terminal
static void doPrint(std::string output) {
  char * out = getenv("LDAUDIT_OUTFILE");
    
  // Open for append to write, and then close
  // This is obviously not ideal for many writes, but I don't
  // see an easy way to do this without having an exit function.
  if (out) {
     std::ofstream file;
     file.open(out, std::ios_base::app);
     if (!file) {
        printf("Error writing to output file.\n");
        exit (EXIT_FAILURE);
     }
     file << output;
     file.close();
  } else {
      std::cout << output;
  }
}

// Prepare output file for writing
__attribute__((constructor))
static void init(void) {
    // Do we want to write output to a file?
    char * out = getenv("LDAUDIT_OUTFILE");
    
    // Prepare file for writing
    if (out) {
        initFile(out);
    }
}

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
unsigned int la_version(unsigned int version) {
  // If version == 0 the library will be ignored by the linker.
  if (version == 0) {
    return version;
  } 

  // Prepare output - top of the yaml, and first event (version)
  std::string output = "auditlib:\n  la_version: " + std::to_string(version) + "\n  audits:\n";
  output += "  - event: handshake\n    function: la_version\n    value: " + std::to_string(version) + "\n";
  doPrint(output);

  return LAV_CURRENT;
}

/*
    The dynamic linker invokes this function to inform the auditing
    library that it is about to search for a shared object.  The name
    argument is the filename or pathname that is to be searched for.
    cookie identifies the shared object that initiated the search.
    flag is set to one of the following values:

    LA_SER_ORIG
           This is the original name that is being searched for.
          Typically, this name comes from an ELF DT_NEEDED entry, or
          is the filename argument given to dlopen(3).

   LA_SER_LIBPATH
          name was created using a directory specified in
          LD_LIBRARY_PATH.

    LA_SER_RUNPATH
          name was created using a directory specified in an ELF
          DT_RPATH or DT_RUNPATH list.

   LA_SER_CONFIG
          name was found via the ldconfig(8) cache
          (/etc/ld.so.cache).

   LA_SER_DEFAULT
          name was found via a search of one of the default
          directories.

   LA_SER_SECURE
          name is specific to a secure object (unused on Linux).

   As its function result, la_objsearch() returns the pathname that
   the dynamic linker should use for further processing.  If NULL is
   returned, then this pathname is ignored for further processing.
   If this audit library simply intends to monitor search paths,
   then name should be returned.
*/
char * la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag) {

  // Derive a human friendly flag name
  std::string flagName;
  std::string desc;
  switch(flag) {
    case LA_SER_ORIG:
      desc = "This is the original name that is being searched for (ELF NEEDED or filename)";
      flagName = "LA_SER_ORIG";
      break;
    case LA_SER_LIBPATH:
      flagName = "LA_SER_LIBPATH";
      desc = "name was created using a directory specified in LD_LIBRARY_PATH";
      break;
    case LA_SER_RUNPATH:
      flagName = "LA_SER_RUNPATH";
      desc = "name was created using a directory specified in an ELF DT_RPATH or DT_RUNPATH list.";
      break;
    case LA_SER_DEFAULT:
      flagName = "LA_SER_DEFAULT";
      desc = "name was found via a search of one of the default directories.";
      break;
    case LA_SER_CONFIG:
      flagName = "LA_SER_CONFIG";
      desc = "name was found via the ldconfig(8) cache (/etc/ld.so.cache).";
      break;
    case LA_SER_SECURE:
      desc = "name is specific to a secure object (unused on Linux).";
      flagName = "LA_SER_SECURE";
  }
  
  std::stringstream ss;
  ss << cookie;
   
  std::string output = "  - event: searching_for\n    function: la_objsearch\n    name: \"" + std::string(name) + "\"\n";
  output += "    initiated_by: " + ss.str() + "\n    flag: \"" + flagName + "\"\n    description: \"" + desc + "\"\n";
  doPrint(output);

  return (char*)name;
}

/*
    The dynamic linker calls this function to inform the auditing
    library that link-map activity is occurring.  cookie identifies
    the object at the head of the link map.  When the dynamic linker
    invokes this function, flag is set to one of the following
    values:

    LA_ACT_ADD
           New objects are being added to the link map.

    LA_ACT_DELETE
          Objects are being removed from the link map.

    LA_ACT_CONSISTENT
          Link-map activity has been completed: the map is once
          again consistent.
*/
void la_activity (uintptr_t *cookie, unsigned int flag) {

  std::string flagName;
  std::string desc;
  switch(flag) {
    case LA_ACT_CONSISTENT:
      flagName = "LA_ACT_CONSISTENT";
      desc = "Link-map activity has been completed (map is consistent)";
      break;
    case LA_ACT_ADD:
      flagName = "LA_ACT_ADD";
      desc = "New objects are being added to the link map.";
      break;
    case LA_ACT_DELETE:
      flagName = "LA_ACT_DELETE";
      desc = "Objects are being removed from the link map.";
      break;
    default:
      flagName = "???";
    }

  std::stringstream ss;
  ss << cookie;
   
  std::string output = "  - event: activity_occurring\n    function: la_activity\n    initiated_by: " + ss.str() + "\n    flag: " + flagName + "\n    description: "+ desc +"\n";
  doPrint(output);
}

/*
    The dynamic linker calls this function when a new shared object
    is loaded.  The map argument is a pointer to a link-map structure
    that describes the object.  The lmid field has one of the
    following values

    LM_ID_BASE
          Link map is part of the initial namespace.

    LM_ID_NEWLM
          Link map is part of a new namespace requested via
          dlmopen(3).

    cookie is a pointer to an identifier for this object.  The
    identifier is provided to later calls to functions in the
    auditing library in order to identify this object.  This
    identifier is initialized to point to object's link map, but the
    audit library can change the identifier to some other value that
    it may prefer to use to identify the object.

    As its return value, la_objopen() returns a bit mask created by
    ORing zero or more of the following constants, which allow the
    auditing library to select the objects to be monitored by
    la_symbind*():

    LA_FLG_BINDTO
          Audit symbol bindings to this object.

    LA_FLG_BINDFROM
          Audit symbol bindings from this object.

    A return value of 0 from la_objopen() indicates that no symbol
    bindings should be audited for this object.
*/
unsigned int la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie) {
 
  std::string flagName;
  std::string linkType;
  switch(lmid) {
    case LM_ID_BASE:
      flagName = "LM_ID_BASE";
      linkType = "Link map is part of the initial namespace";
      break;
    case LM_ID_NEWLM:
      flagName = "LM_ID_NEWLM";
      linkType = "Link map is part of a new namespace requested via dlmopen(3).";
      break;
    default:
      flagName = "???";
      linkType = "Unknown";
  }

  std::stringstream ss;
  ss << cookie;
   
  std::string output = "  - event: object_loaded\n    name: \"" + std::string(map->l_name) + "\"\n    function: la_objopen\n    identifier: " + ss.str() + "\n    flag: " + flagName + "\n    description: " + linkType +"\n";
  doPrint(output);
  return LA_FLG_BINDTO | LA_FLG_BINDFROM;
}


/*

    The dynamic linker invokes this function after any finalization code
    for the object has been executed, before the object is unloaded. 
    The cookie argument is the identifier obtained from a previous 
    invocation of la_objopen().

    In the current implementation, the value returned by la_objclose() is ignored. 
*/
unsigned int la_objclose (uintptr_t *cookie) {
    printf("  - event: object_closed\n    function: la_objclose\n    identifier: %p\n", cookie);
    return 0;
} 
