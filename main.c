#include <windows.h>
#include "zip.h"

void zip_walk(struct zip_t *zip, const char *path, size_t base_len, const char *out_zip);

int main(int argc, const char** argv) {
  if (argc < 3) {
    return -1;
  }

  char out_zip[MAX_PATH];
  char src_dir[MAX_PATH];

  GetFullPathName(argv[2], MAX_PATH, out_zip, NULL);
  GetFullPathName(argv[1], MAX_PATH, src_dir, NULL);

  struct zip_t *zip = zip_open(out_zip, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
  if (!zip) return -1;

  size_t base_len = strlen(src_dir);
  
  zip_walk(zip, src_dir, base_len, out_zip);

  zip_close(zip);
  return 0;
}

void zip_walk(struct zip_t *zip, const char *path, size_t base_len, const char *out_zip) {
  WIN32_FIND_DATA findFileData;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  char searchPath[MAX_PATH];

  char fullPath[MAX_PATH];
  char fullPathAbs[MAX_PATH];

  snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
  hFind = FindFirstFile(searchPath, &findFileData);
  
  if (hFind == INVALID_HANDLE_VALUE) return;

  do {
    if (strcmp(findFileData.cFileName, ".") == 0 || 
        strcmp(findFileData.cFileName, "..") == 0) {
      continue;
    }

    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, findFileData.cFileName);

    GetFullPathName(fullPath, MAX_PATH, fullPathAbs, NULL);

    if (strcmp(fullPathAbs, out_zip) == 0) {
      continue;
    }

    if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      zip_walk(zip, fullPath, base_len, out_zip);
    } else {
      const char *zip_entry_name = fullPath + base_len;
      
      while (*zip_entry_name == '/' || *zip_entry_name == '\\') {
        zip_entry_name++;
      }

      zip_entry_open(zip, zip_entry_name);
      zip_entry_fwrite(zip, fullPath);
      zip_entry_close(zip);
    }
  } while (FindNextFile(hFind, &findFileData) != 0);

  FindClose(hFind);
}