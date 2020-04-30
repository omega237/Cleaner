#pragma once

#ifndef __FILE_HELPER_H__
#define __FILE_HELPER_H__

#include <windows.h>

/*
 * FileHelper_DeleteFile
 * deletes a windows file system object to recycle bin or from hard disk
 * NOTE: What happens if recycle bin is full but you want to delete to it?????? Exceptions not handled...
 *
 * @param File: points to buffer of file(s) to delete, has to be double null terminated and files seperated by null
 * @param Delete: move to recycle bin or delete
 * 
 * @return: true
 */
bool FileHelper_DeleteFile(char *File, bool Delete = false, bool Ask = false)
{
  SHFILEOPSTRUCT shFileOp;
  shFileOp.hwnd = NULL;
  shFileOp.wFunc = FO_DELETE;
  shFileOp.pFrom = File;
  shFileOp.pTo = NULL;
  if(!Ask)
  { shFileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION ;}
  else
  { shFileOp.fFlags = FOF_SILENT; }

  if(!Delete)
  { shFileOp.fFlags |= FOF_ALLOWUNDO; }

  return (SHFileOperation(&shFileOp)==0?true:false);

}

#endif // __FILE_HELPER_H__
