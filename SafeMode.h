#pragma once

#include <windows.h>
#include <string.h>
#include <commctrl.h>
#include "FileHelper.h"
#include "StringHelper.h"
#include "resource.h"

// global vars for safe mode
char safeDir[MAX_PATH];
char safeMask[MAX_PATH];
bool safeRec = false;
bool safeIgnore = false;
bool safeDel = false;

HINSTANCE g_hInstance;
HWND hwndLv = NULL;
HWND StaticPath = NULL;
HWND StaticMask = NULL;
HWND StaticNum = NULL;
HWND StaticSize = NULL;
HWND hwndsw = NULL;

bool g_OptionsChanged = false;

void SearchAndFill(char *srchfor, HWND hwndlv)
{
  static int itmcnt = 0;
  WIN32_FIND_DATA wfd;
  HANDLE find;
  find = FindFirstFile(srchfor, &wfd);
  if(find != INVALID_HANDLE_VALUE)
  {
    // nachglotzen was er als erstes gefunden hat
    // wenn dir und rec dann mit dem neu aufrufen
    // attribute ignorieren oder nicht
    // sonst lv füllen
    // here goes da search
    while(FindNextFile(find, &wfd))
    {
      LV_ITEM LvItem;
      memset(&LvItem,0,sizeof(LvItem)); // Zero struct's Members
      //  Setting properties Of members:
      LvItem.mask=LVIF_TEXT;   // Text Style
      LvItem.cchTextMax = 1000; // Max size of test
      LvItem.iItem=itmcnt;          // choose item  
      LvItem.iSubItem=0;       // Put in first coluom
      LvItem.pszText=wfd.cFileName; // Text to display (can be from a char variable) (Items)
      //SendMessage(hwndLv,LVM_INSERTITEM,0,(LPARAM)&LvItem); // Send info to the Listview
      LvItem.mask=LVIF_TEXT;   // Text Style
      LvItem.cchTextMax = 1000; // Max size of test
      LvItem.iItem=-1;          // choose item  
      LvItem.iSubItem = 0;
      LvItem.pszText=wfd.cFileName;
      SendMessage(hwndLv,LVM_INSERTITEM,0,(LPARAM)&LvItem); // Send info to the Listview
      itmcnt++;
      SetDlgItemText(hwndsw, IDC_PATH, wfd.cFileName);
    }
  }
  FindClose(find);
}


/*
 * GetHeaderFont
 * Creates font for Main Dlg Header
 *
 * @return: font
 */
HFONT GetHeaderFont()
{
  LOGFONT cn;
  cn.lfHeight         = -32;
  cn.lfWidth          = 0;
  cn.lfEscapement     = 0;
  cn.lfOrientation    = 0;
  cn.lfWeight         = 700;
  cn.lfItalic         = 0;
  cn.lfUnderline      = 0;
  cn.lfStrikeOut      = 0;
  cn.lfCharSet        = 0;
  cn.lfOutPrecision   = 1;
  cn.lfClipPrecision  = 2;
  cn.lfQuality        = 1;
  cn.lfPitchAndFamily = 34;
  strcpy(cn.lfFaceName,"MS Sans Serif");
  return CreateFontIndirect(&cn);
};

/* 
 * SearchingProc
 * The search dialogues window procedure
 *
 * @params : See CleanerProc
 * @returns: See CleanerProc
 */
LRESULT CALLBACK SearchingProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  switch(Message)
  {
    case WM_INITDIALOG:
      return 0;
    break;
    case WM_SHOWWINDOW:
      return 0;
      break;
    default:
      return 0;//DefWindowProc(hwnd, Message, wParam, lParam);
  }
  return false;
};

/*
 * PopulateListView
 * Shows up search progress dialogue and fills listview in main dlg
 */
inline void PopulateListView(HWND hwnd)
{
  hwndsw = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_SEARCHING), hwnd, (DLGPROC)SearchingProc);
  //DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_SEARCHING), hwnd, (DLGPROC)SearchingProc, 0);
  EnableWindow(hwnd, false);
  // searching goes here
  // call searchforfiles for each mask in cmd line
  int cnt = GetMaskPart(-1, safeMask, NULL);
  SearchAndFill("C:\\*.txt", hwndLv);
  // end searching
  EndDialog(hwndsw, 0);
  EnableWindow(hwnd, true);
};

/* 
 * OptionsProc
 * The options dialogues window procedure
 *
 * @params : See CleanerProc
 * @returns: See CleanerProc
 */
LRESULT CALLBACK OptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  // bei doppelklick alle felder enablen und changed auf true setzen
  // bei ok wenn optionschanged, alles in die vars kopieren und EndDialog(hwnd, 1)
  switch(Message)
  {
    case WM_LBUTTONDBLCLK:
      g_OptionsChanged = true;
      // schreibschutz für die editfelder aufheben und die anderen Fenster aktivieren
      EnableWindow(GetDlgItem(hwnd, IDC_ORECURSE), true);
      EnableWindow(GetDlgItem(hwnd, IDC_ODELETE), true);
      EnableWindow(GetDlgItem(hwnd, IDC_OMASK), true);
      EnableWindow(GetDlgItem(hwnd, IDC_OIGNORE), true);
      break;
    case WM_INITDIALOG:
      // init listview and fill with search results IDC_FILELIST
      SetDlgItemText(hwnd, IDC_OPATH, safeDir);
      SetDlgItemText(hwnd, IDC_OMASK, safeMask);
      if(safeRec) SendMessage(GetDlgItem(hwnd, IDC_ORECURSE),BM_SETCHECK, (WPARAM)BST_CHECKED	, 0);
      if(safeDel) SendMessage(GetDlgItem(hwnd, IDC_ODELETE),BM_SETCHECK, (WPARAM)BST_CHECKED	, 0);
      if(safeIgnore) SendMessage(GetDlgItem(hwnd, IDC_OIGNORE),BM_SETCHECK, (WPARAM)BST_CHECKED	, 0);
      return 0;
    break;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          if(g_OptionsChanged)
          { 
            safeRec = (SendMessage(GetDlgItem(hwnd, IDC_ORECURSE), BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
            safeDel = (SendMessage(GetDlgItem(hwnd, IDC_ODELETE), BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
            safeIgnore = (SendMessage(GetDlgItem(hwnd, IDC_OIGNORE), BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
            GetDlgItemText(hwnd, IDC_OPATH, safeDir, 100);
            GetDlgItemText(hwnd, IDC_OMASK, safeMask, 100);
            EndDialog(hwnd, 1); 
          } // return that search options have changed
          else
          { EndDialog(hwnd, 0); }
          g_OptionsChanged = false;
        break;
      }
    break;
    case WM_CLOSE:
      EndDialog(hwnd, 0);
      g_OptionsChanged = false;
      break;
    default:
      return 0;//DefWindowProc(hwnd, Message, wParam, lParam);
  }
  return false;
};

/*
 * CleanerProc
 * Cleaner safe mode dialogue message procedure
 *
 * @param hwnd: handle of window
 * @param Message: message code
 * @param wParam: wParam of Message
 * @param lParam: lParam of Message
 *
 * @return: LRESULT
 */
LRESULT CALLBACK CleanerProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  switch(Message)
  {
    case WM_INITDIALOG:
      SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION)));
      SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION)));
      SendMessage(GetDlgItem(hwnd, IDC_HEADER), WM_SETFONT, (WPARAM) GetHeaderFont(), (LPARAM)true);
      hwndLv = GetDlgItem(hwnd, IDC_FILELIST);
      StaticMask = GetDlgItem(hwnd, IDC_MASK);
      StaticPath = GetDlgItem(hwnd, IDC_PATH);
      StaticNum = GetDlgItem(hwnd, IDC_NUMFILES);
      StaticSize = GetDlgItem(hwnd, IDC_FILESIZE);
      // init listview and fill with search results IDC_FILELIST
      LV_COLUMN lvc;
      lvc.mask = LVCF_FMT	| LVCF_TEXT | LVCF_WIDTH;
      lvc.fmt = LVCFMT_LEFT;
      lvc.cx = 300;
      lvc.pszText = "File";
      ListView_InsertColumn(hwndLv, 0, &lvc);
      lvc.cx = 50;
      lvc.pszText = "Size";
      ListView_InsertColumn(hwndLv, 1, &lvc);
	  PopulateListView(hwnd);
      return 0;
    break;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          EndDialog(hwnd, 0);
          PostQuitMessage(0); // to end message loop
        break;
        case IDCANCEL:
          EndDialog(hwnd, 0);
          PostQuitMessage(0); // to end message loop
          break;
        case IDC_SHOWOPTIONS:
          int Searchagain;
          Searchagain = DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hwnd, (DLGPROC)OptionsProc, 0);
          if(Searchagain > 0)
          { PopulateListView(hwnd); }
          break;
      }
    break;
    default:
      return 0;//DefWindowProc(hwnd, Message, wParam, lParam);

  }
  return false;
};

/*
 * BeginSafeMode
 * starts graphical execution of deletion mode
 * 
 * @param Dir: directory to begin with
 * @param Mask: search mask
 * @param rec: recursive work or not
 * @param ignore: ignore writeprotection etc
 * @param del: delete files
 */
void BeginSafeMode(char *Dir, char *Mask, bool rec, bool ignore, bool del, HINSTANCE hInstance)
{
  MSG msg;
  HWND DlgBox;
  // asserten alle ptr != NULL
  g_hInstance = hInstance;
  InitCommonControls();
  strcpy(safeDir, Dir);
  strcpy(safeMask, Mask);
  safeRec = rec; 
  safeIgnore = ignore;
  safeDel = del;
  DlgBox = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_CLEANERDLG), NULL, (DLGPROC)CleanerProc, NULL);
  if(DlgBox==NULL)
  {
    MessageBox(NULL, "Window creation failed.\nExiting Cleaner...", "Cleaner Version 1.0", MB_OK | MB_ICONERROR);
    return;
  }
  ShowWindow(DlgBox, SW_SHOW);
  UpdateWindow(DlgBox);
  while(GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
};