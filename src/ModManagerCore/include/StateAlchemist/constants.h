#if !defined(MYLIB_CONSTANTS_H)
#define MYLIB_CONSTANTS_H 1

#include <string>
#include <set>


const std::string APP_VERSION = "1.1.4-sehling";

const int64_t FILE_LIST_BUFFER_SIZE = 100;

// Max number of folders/files to load at a time when processing them
const int MAX_FS_ENTRY_LOAD = 50;

// Substring to delimit the rating from the mod name in the folder name:
const std::string RATING_DELIMITER = "~~";

const std::string TXT_EXT = ".txt";
const std::string ALCHEMIST_FOLDER = "mod_alchemy";
const std::string ALCHEMIST_PATH = "/" + ALCHEMIST_FOLDER;
const std::string ATMOSPHERE_PATH = "/atmosphere/contents/";

// UI seems likely to hang if a control's label is much longer than this number
const int MAX_LABEL_SIZE = 25;

// Just some symbols that I think libnx's filesystem operations will allow in folder and file names.
//
// TODO: I just came up with these on a wim. There's probably more that can be added to these.
//       Maybe a couple of these will even cause errors. It would be nice to have this set be more definitive.
const std::set<char> ALLOWED_FOLDER_SYMBOLS = {'-', '_', '(', ')', '#', '%', '&', '!', '+', '='};

#endif
