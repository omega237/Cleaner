/******************************************************************************
Module:  CmdLine.h
Notices: Copyright (c) 2004 Oliver Koch
Purpose: Header file containing a class to easily handle a programs cmdline
Version: 1.0
Changes: -
Notes:   -> als nächstes die Zahl der mitzunehmenden attribute für einen
            schalter festlegen
         -> errorcodes einfügen, bzw messages... abrufbar über get errormsg
         -> erst die ganze commandline lowercase, bevor die bearbeitung beginnt
         -> was ist mit tippfehlern in der kommandozeile????
         -> parse und zählfunktionalität in eine funktion mit param true wenn parse
         -> vorm überprüfen auf unbekannte parameter prüfen!!!!!!!
Usage:   
To use the CommandLineParser facility, first create an array of CmdLineArgDesc
and fill its elements with the method FillInitStruct. Do the same with CmdLineArgDesc.
Note that it is better to save the rules in order of UNIQUE first, REQUIRED second, EXCLUDE third.
Then pass your arrays and the command line to ParseCmdLine
******************************************************************************/

#pragma once // include only one time

#include <windows.h>
#include <string.h>
#include "StringHelper.h"

#define ARRNUMEL(Array) ((sizeof(Array))/(sizeof(Array[0])))  

#define MAX_ARG_LENGTH 256

#define CMDLINE_RULE_UNIQUE     1
#define CMDLINE_RULE_REQUIRED   2
#define CMDLINE_RULE_EXCLUDE    3

namespace CmdLineParser // namespace CmdLineParser to avoid naming conflicts
{

 /*
  * CmdLineArgDesc
  * structure used for CmdLineParser initialization
  *
  * @member CmdLineSwitch: simple form of desired switch
  * @member CmdLineSwitchAlt: long form of desired switch
  * @member CmdLineHelpText: help text for switch(es)
  * @member SwitchSet: points to bool that will determine if switch was set
  * @member RequireAttr: determines if switch needs an additional param
  * @member SwitchArg: points to buffer that will hold switch argument
  * @member SwitchArgName: points to switch argument description
  *
  */
  struct CmdLineArgDesc
  {
    char *CmdLineSwitch;
    char *CmdLineSwitchAlt;
    char *CmdLineHelpText;
    bool *SwitchSet;
    bool  RequireAttr;
    char *SwitchAttr;
    char *SwitchAttrName;
  };

 /*
  * ArgRuleSet
  * structure used for argument pairing rules etc
  *
  * @member Switch: points to CmdLineArgDesc struct that is affectedt by rule
  * @member RuleMode: rule mode
  * @member RuleExtra: points to CmdLineArgDesc struct if CMDLINE_RULE_EXCLUDE 
  *
  */
  struct ArgRuleSet
  {
    CmdLineArgDesc *Switch;
    int             RuleMode;
    CmdLineArgDesc *RuleExtra;
  };
 
 /* 
  * CommandLineParser
  * class for working with command line switches
  *
  */
  class CommandLineParser
  {
  private:

    int argc; // argument count
    char **argv; // argument values


    bool IsUnique(char *Which, ArgRuleSet ars[], int arsEls = 0)
    {
      for(int i=0; i<arsEls; i++)
      {
        if(((!strcmp(ars[i].Switch->CmdLineSwitch, Which))||(!strcmp(ars[i].Switch->CmdLineSwitchAlt, Which)))&&(ars[i].RuleMode == CMDLINE_RULE_UNIQUE))
        { return true; }
      }
      return false;
    };

   /*
    * CleanUp
    * cleans argc and argv, frees mem
    */
    inline void CleanUp()
    {
      for(int i=0; i<argc; i++)
      { delete argv[i]; }
      argc = 0;
    };


   /*
    * CheckParamCount
    * checks if there are enough params
    *
    * @param clad:
    * @param cladEls:
    *
    * @return:
    */
    bool CheckParamCount(CmdLineArgDesc clad[], int cladEls)
    {
      int num = 0;
      for(int i=0; i<cladEls; i++)
      {
        for(int j=0; j<argc; j++)
        {
          if((!strcmp(clad[i].CmdLineSwitch, argv[j]))||(!strcmp(clad[i].CmdLineSwitchAlt, argv[j])))
          { num += (1+(clad[i].RequireAttr?1:0)); }
        }
      } 
      if(num==argc)
      { return true; }
      return false;
    };

   /*
    * ApplySwitches
    * applies the switch values to the assigned variables
    *
    * @param clad: switch descriptions
    * @param cladEls: number of switches in clad
    *
    * @return: true, false
    */
    bool ApplySwitches(CmdLineArgDesc clad[], int cladEls)
    {
      // run through argv and compare with clad to get data for param
      for(int i=0; i<argc; i++)
      {
        for(int j=0; j<cladEls; j++)
        {
          if((!strcmp(argv[i], clad[j].CmdLineSwitch))||
             (!strcmp(argv[i], clad[j].CmdLineSwitchAlt)))
          {
            //ASSERT clad pointers != null
            *(clad[j].SwitchSet) = true;
            if(clad[j].RequireAttr && i<argc)
            {
              strcpy(clad[j].SwitchAttr, argv[i+1]);
              i++;
            }
            else if (clad[j].RequireAttr && i>=argc)
            { return false; }
          }
        }
      }
      return true;
    }

   /* 
    * CheckRules
    * checks if everything is right according the rules
    *
    * @param ars: cmdline arguments ruleset
    * @param arsEls: number of elements in ars list
    *
    * @return: true or false on rule violation
    */
    bool CheckRules(ArgRuleSet ars[], int arsEls = 0)
    {
      // check for unique rule violations
      for(int i=0; i<argc; i++)
      {
        for(int j=0; j<arsEls; j++)
        {
          if(ars[j].RuleMode == CMDLINE_RULE_UNIQUE)
          {
            if((!strcmp(argv[i], ars[j].Switch->CmdLineSwitch) ||
                !strcmp(argv[i], ars[j].Switch->CmdLineSwitchAlt)) && 
                (argc > (1+(ars[j].Switch->RequireAttr?1:0))))
            {
              return false;
            }
          }
        }
      }
      // check for required rule violations
      for(int k=0; k<arsEls; k++)
      {
        bool reqfound = false;
        if(ars[k].RuleMode == CMDLINE_RULE_REQUIRED)
        {
          for(int l=0; l<argc; l++)
          {
            if((!strcmp(argv[l], ars[k].Switch->CmdLineSwitch)||
               !strcmp(argv[l], ars[k].Switch->CmdLineSwitchAlt))&&
               argc >= (1+(ars[k].Switch->RequireAttr?1:0)))
            { reqfound = true; }
          }
        }
        if(!reqfound && (ars[k].RuleMode == CMDLINE_RULE_REQUIRED))
        { if((argc > 1) || !IsUnique(argv[0], ars, arsEls)) return false; }
      }
      // check for exclude rule violations

      return true;
    };

   /*
    * GetNumParams
    * counts the number of params in cmdLine
    *
    * @param cmdLine: point to cmdline to count parameters from
    *
    * @return: number of params in cmdLine
    */
    int GetNumParams(char *CmdLine)
    {
      int cmdlength = strlen(CmdLine);
      int i = 0, numparams = 0;;
      do // analyze cmd line char by char from beginning to end
      {
        int j = 0;
        do // every element
        {
          if(CmdLine[i] == '"') // copy until next " found or i >= cmdlength
          {
            i++;
            do
            {
              i++; j++;
            }
            while(CmdLine[i] != '"' && i<cmdlength);
            i++;
            break;
          } // " case end
          i++; j++;
        }
        while(CmdLine[i] != ' ' && i<cmdlength); // i<cmdlength to prevent program reading over str when string not terminated with second "
        i++; // skip space
        numparams++;
      }
      while(i<cmdlength);    
      return numparams;
    };

  public:
 
   /*
    * FillInitStruct
    * used to easily fill CmdLineArgDesc
    *
    * @param params in same order as CmdLineArgDesc members
    * @param clad: init struct to fill
    *
    * @return: always true
    */
    bool FillInitStruct(char *Switch, char *SwitchAlt, char *Help, bool *Set,
                        bool RequireAttr, char *Attr, char* AttrName, CmdLineArgDesc *clad)
    {
      // ASSERT((Switch!=NULL&&Help!=NULL&&Set!=NULL&&Arg!=NULL&&clad!=NULL),
      // "not all arguments have a value");

      // alloc mem
      clad->CmdLineSwitch = new char[strlen(Switch)+1];
      clad->CmdLineSwitchAlt = new char[strlen(SwitchAlt)+1];
      clad->CmdLineHelpText = new char[strlen(Help)+1];
      if(RequireAttr)
      { clad->SwitchAttrName = new char[strlen(AttrName)+1]; }

      // assert newly allocated mem
      // copy data to struct
      strcpy(clad->CmdLineSwitch, Switch);
      strcpy(clad->CmdLineSwitchAlt, SwitchAlt);
      strcpy(clad->CmdLineHelpText, Help);
      clad->SwitchSet = Set;
      clad->RequireAttr = RequireAttr;
      clad->SwitchAttr = Attr;
      if(RequireAttr)
      { strcpy(clad->SwitchAttrName, AttrName); }
    
      return true;
    };

   /*
    * FillRuleStruct
    * used to easily fille ArgRuleSet structure
    * 
    * @param affects: points to CmdLineArgDesc affected by rule
    * @param mode: rule mode
    * @param ext: points to CmdLineArgDesc if mode = 2
    * @param ars: ArgRuleSet to fill
    *
    * @return: always true
    */
    bool FillRuleStruct(CmdLineArgDesc *affects, int mode, CmdLineArgDesc *ext,
                        ArgRuleSet *ars)
    {
      // ASSERT pt and ext if mode=2 and ars
      ars->Switch = affects;
      ars->RuleMode = mode;
      ars->RuleExtra = ext;
      return true;
    };

   /*
    * GetProgramUsage
    * creates the programs parameter information based on clad
    *
    * @param ProgName: the programs executable file name \{.exe}
    * @param ProgPurpose: programs general purpose
    * @param lpBuf: buffer, the usage information is copied into (must be large enough)
    * @param clad: argument info
    * @param numEls: number of elements in clad
    *
    * @return: always true
    */
    bool GetProgramUsage(char *ProgName, char *ProgPurpose, char *lpBuf, 
                        CmdLineArgDesc clad[], int numEls = 0)
    {
      char CmdLineText[1024];
      char UsageText[5120];

      // assert

      int numclad = numEls;

      // create command line text
      strcpy(CmdLineText, ProgName);
      strcat(CmdLineText, " ");
      for(int i=0; i<numclad; i++)
      {
        char pi[100]; // parameter info
        if(clad[i].RequireAttr)
        { wsprintf(pi, "[%s [%s]] ", clad[i].CmdLineSwitch, clad[i].SwitchAttrName); }
        else
        { wsprintf(pi, "[%s] ", clad[i].CmdLineSwitch); }
        strcat(CmdLineText, pi);
      }
      strcat(CmdLineText, "\n");

      // create usage text
      strcpy(UsageText, "\n");
      for(int j=0; j<numclad; j++)
      {
        char usageline[500];
        wsprintf(usageline, "%s,%s: %s", clad[j].CmdLineSwitch, 
                 clad[j].CmdLineSwitchAlt, clad[j].CmdLineHelpText);
        strcat(usageline, "\n");
        strcat(UsageText, usageline);
      }

      // copy to buf
      strcpy(lpBuf, ProgPurpose);
      strcat(lpBuf, "\n");
      strcat(lpBuf, CmdLineText);
      strcat(lpBuf, UsageText);

      return true;
    };

   /*
    * ParseCmdLine
    * parses the cmdline and saves values to dos style argc, arv
    *
    * @param CmdLine: pointer to string to parse
    * @param clad: CmdLineArgDesc array
    * @param ars: ArgRuleSet
    * @param cladEls: number of elements in clad array
    * @param arsEls: number of elements in ars array
    *
    * @return: true, false on error or rule violation
    */
    bool ParseCmdLine(char *CmdLine, CmdLineArgDesc clad[], ArgRuleSet ars[],
                      int cladEls = 0, int arsEls = 0)
    {
      // first get num of paramters in cmdline and allocate mem for argv pointer array
      int numpar = GetNumParams(CmdLine);
      argv = new char*[numpar];


      int cmdlength = strlen(CmdLine);
      int i = 0;
      do // analyze cmd line char by char from beginning to end
      {
        char tmp[100];
        int j = 0;
        do // every element
        {
          if(CmdLine[i] == '"') // copy until next " found or i >= cmdlength
          {
            i++;
            do
            {
              tmp[j] = CmdLine[i];
              i++; j++;
            }
            while(CmdLine[i] != '"' && i<cmdlength);
            i++;
            break;
          } // " case end
          tmp[j] = CmdLine[i];
          i++; j++;
        }
        while(CmdLine[i] != ' ' && i<cmdlength); // i<cmdlength to prevent program reading over str when string not terminated with second "
        tmp[j] = '\0';
        i++; // skip space
        // vorm kopieren die führenden spaces eliminieren und wenn der string nur spaces enthält, dann nicht kopieren und aufnehmen!!!!!!!!!!
        if(!IsStringBlank(tmp)){
          StripLeadingBlanks(tmp);
          argv[argc] = (char*) new char[strlen(tmp)+1]; // siehe oben
          if(argv[argc] == NULL)
          { return false; } // Clean UP !
          strcpy(argv[argc], tmp);
          argc++;
        }
      }
      while(i<cmdlength);

      // erwartete parameterzahl gegen tatsächliche parameterzahl prüfen und wenn die beiden zahlen gleich sind: OK!
      if(!CheckParamCount(clad, cladEls))
      { return false; }

      bool ret = CheckRules(ars, arsEls); // wenn false, argc=0 and argv = NULL
      ret &= ApplySwitches(clad, cladEls);
      if(ret==false)
      { CleanUp(); }
      return ret;
    };

   /*
    * CommandLineParser
    * standard constructor of class CommandLineParser
    */
    CommandLineParser()
    {
      argc = 0;
    };

   /*
    * ~CommandLineParser
    * standard destructor of class CommandLineParser
    */
    ~CommandLineParser()
    {
      CleanUp();
    };

  }; // end of class

}; // end of namespace