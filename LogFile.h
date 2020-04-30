/******************************************************************************
Module:  LogFile.h
Notices: Copyright (c) 2004 Oliver Koch
Purpose: Header file containing a class to easily write lines of text to a file
Version: 1.0
Changes: -
Notes:   -> log und format finktionen hinzufügen
******************************************************************************/

#pragma once // include only one time

#include <windows.h>
#include <string.h>
#include <fstream.h>

namespace Logger // namespace Logger to avoid naming conflicts
{

 /* 
  * LogFile
  * class for writing logfiles
  *
  */
  class LogFile
  {
  private:
  protected:
  public:

   /* 
    * WriteLine
    * writes a line to the specified file
    *
    * @param LogFileName: file to write to
    * @param LogMessage: message to write
    */
    void WriteLine(char *LogFileName, char *LogMessage)
    {
      // assert pointers != NULL
      return;
    }

   /*
    * LogFile
    * standard constructor
    */
    LogFile()
    {};

   /*
    * ~LogFile
    * standard destructor
    */
    ~LogFile()
    {};
  };

}; // end of namespace
