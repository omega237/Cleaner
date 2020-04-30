////////////// Allow code to compile cleanly at warning level 4 ///////////////
///// code taken from Programming Windows 4th. Edition by Jeffrey Richter /////

/* nonstandard extension 'single line comment' was used */
#pragma warning(disable:4001)

// unreferenced formal parameter
#pragma warning(disable:4100)

// Note: Creating precompiled header 
#pragma warning(disable:4699)

// function not inlined
#pragma warning(disable:4710)

// unreferenced inline function has been removed
#pragma warning(disable:4514)

// assignment operator could not be generated
#pragma warning(disable:4512)

///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "CmdLine.h"
#include "LogFile.h"
#include "SafeMode.h"
#include "FileHelper.h"

using namespace CmdLineParser;
using namespace Logger;

const char g_ProgramName[] = "Cleaner Version 1.0";
LogFile g_Log;

/* 
 * Command Line switch variables
 */
bool ShowHelp = false;
bool RunRecursive = false;
bool IgnoreAttributes = false;
bool Xterminate = false;
bool SafeMode = false;
bool LogOutput = false;
char LogFileName[MAX_ARG_LENGTH];
bool DelMask = false;
char Mask[MAX_ARG_LENGTH];
bool DirSet = false;
char Dir[MAX_ARG_LENGTH];

char UsageMessage[1000];

/*
 * PrepareExit
 * prepares for application exit, frees mem etc
 */
void PrepareExit()
{
};

/*
 * InitAndParseCmdLine
 * initializes param descriptions and rules and parses cmd line
 * 
 * @param CmdLine: string to parse as cmd line
 *
 * @return true if ok
 */
bool InitAndParseCmdLine(char *CmdLine)
{
  CmdLineArgDesc clad[8];
  ArgRuleSet ars[4];
  CommandLineParser clp;

  // Init command line parameters
  clp.FillInitStruct("-h", "--help", "\t\tShows this help screen, will return error if used with other switches", &ShowHelp, false, NULL, NULL, &clad[0]);
  clp.FillInitStruct("-r", "--recursive", "\tDeletes files in the specified directory and its sub-directories", &RunRecursive, false, NULL, NULL, &clad[1]);
  clp.FillInitStruct("-i", "--ignore", "\tIgnores write-protection of files that match with mask", &IgnoreAttributes, false, NULL, NULL, &clad[2]);
  clp.FillInitStruct("-x", "--xterm", "\tDeletes matching files instead of moving them to recycle bin", &Xterminate, false, NULL, NULL, &clad[3]);
  clp.FillInitStruct("-g", "--graphic", "\tUses graphic/safe mode, disables logging!", &SafeMode, false, NULL, NULL, &clad[4]);
  clp.FillInitStruct("-l", "--logfile", "\t\tLogs progress to LogFile", &LogOutput, true, LogFileName, "Logfile", &clad[5]);
  clp.FillInitStruct("-m", "--mask", "\tSpecifies a search mask, this attribute is always required", &DelMask, true, Mask, "mask", &clad[6]);
  clp.FillInitStruct("-d", "--directory", "\tSpecifies the directory to start deletion in, this attribute is always required", &DirSet, true, Dir, "directory", &clad[7]);

  // init command line parameter rules
  clp.FillRuleStruct(&clad[0], CMDLINE_RULE_UNIQUE, NULL, &ars[0]);
  clp.FillRuleStruct(&clad[6], CMDLINE_RULE_REQUIRED, NULL, &ars[1]);
  clp.FillRuleStruct(&clad[7], CMDLINE_RULE_REQUIRED, NULL, &ars[2]);
  clp.FillRuleStruct(&clad[4], CMDLINE_RULE_EXCLUDE, &clad[5], &ars[3]);


  // create usage message and save it globally
  clp.GetProgramUsage("Cleaner.exe", "Cleaner Version 1.0\nFile Deletion Tool", UsageMessage, clad, 8);

  if(!clp.ParseCmdLine(CmdLine, clad, ars, 8, 4))
  {
    MessageBox(NULL, "Wrong Command Line.\nPlease use Cleaner GUI or refer to help.", CmdLine, MB_OK | MB_ICONERROR);
    MessageBox(NULL, UsageMessage, g_ProgramName, MB_OK | MB_ICONERROR );
    PrepareExit();
    return false;
  }
  return true;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR CmdLine, int)
{
  if(!strcmp(CmdLine, ""))
  {
    MessageBox(NULL, "Cleaner Version 1.0 (c) 2004 by Oliver Koch\n\nE-Mail: olliman@gmx.net", g_ProgramName, MB_OK | MB_ICONASTERISK);
    PrepareExit();
    return 0;
  }
  else if(!InitAndParseCmdLine(CmdLine)) // failure in cmdline: exit program
  { 
    PrepareExit();
    return 0; 
  }
  if(ShowHelp)
  { // show help messagebox and exit program
    MessageBox(NULL, UsageMessage, g_ProgramName, MB_OK | MB_ICONASTERISK ); 
    PrepareExit();
    return 0;
  }

  // wenn wir hier angekommen sind, dann können wir mit der arbeit anfangen
  // dann war alles von der kommandozeile her ok und die einstellungen wurden in die entsprechenden variablen gespeichert
  if(SafeMode)
  {
    BeginSafeMode(Dir, Mask, RunRecursive, IgnoreAttributes, Xterminate, hInstance);
  }

  return 0;
};