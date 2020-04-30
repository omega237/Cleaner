#pragma once

#ifndef __STRING_HELPER_H__
#define __STRING_HELPER_H__

/*************************************************************************/
/* ADD GARBAGE COLLECTION TO SPLITSTRING AND FUNCTIONS OF THAT KIND !!!! */
/*************************************************************************/

#include <string.h>

inline char to_lower    (char c) { return (c | 0x20); }; /* converts char to lowercase */
inline char to_upper    (char c) { return (c & 0xDF); }; /* converts char to uppercase */
inline char invert_case (char c) { return (c ^ 0x20); }; /* inverts case of char */
inline bool is_upper    (char c) { return ((c & 0x20) == 0); }; /* char uppercase? */
inline bool is_lower    (char c) { return ((c & 0x20) == 0x20); }; /* char lowercase? */       
inline int  to_int      (char c) { return (c & 0xF); }; /* converts char to int */
inline char to_char     (int  n) { return (n & 0x30); }; /* converts single number to char */

/*
 * lower
 * makes a string lowercase
 *
 * @param sztolower: string that is to be converted
 */
void lower(char *sztolower)
{
  int l = strlen(sztolower);
  for(int i=0; i<l; i++)
  { sztolower[i] = (char)to_lower(sztolower[i]); }
};

/*
 * searchstr
 * searches for occurance of str2 in str1
 *
 * @param str1: string to search in
 * @param str2: string to search for in str1
 * @param cs: case sensitive search
 * @param partstr: if false only searches for str2 at beginning of str1 -> autocomplete
 *
 * @return: true on match
 */
bool searchstr(char *str1, char *str2, bool cs = false, bool partstr = true)
{
  if(str1==NULL || str2==NULL) return false;
  int l1, l2;
  l1 = strlen(str1);
  l2 = strlen(str2);
  if(l2>=l1) return false;
  // principle:
  // search for first match and then try to find rest of str2 in str1
  char *copy = new char[l1+1];
  char *copy1 = new char[l2+1];
  strcpy(copy,str1);
  strcpy(copy1,str2);
  if(!cs)
  {
    lower(copy1);
    lower(copy);
  }
  if(partstr) // search for str2 in whole str1
  {
    bool run = true;
    int idx = 0;
    do
    {
      if(copy[idx]==copy1[0])//erste übereinstimmung
      {        // überprüfen ob der rest des strings auch übereinstimmt
        if(strlen(copy1)==1)return true;
        bool yo = true;
        int off = 1;
        do
        {
          if(copy[idx+off]!=copy1[0+off])yo=false;
          off++;
        }        while((off < l2) && yo);
        if(yo) return yo;
        idx += off-2;
      }
      idx++;
    }    while(run && (idx < l1));
  }
  else // only search at beginning of str2
  {
    int l = strlen(str2);
    for(int m=0; m<l; m++)
    {
      if(copy[m]!=copy1[m]) return false;
    }
    return true;
  }
  return false;
}

/*
 * StripLeadingBlanks
 * strips leading blanks from a string. if string is entirely blank, string is set to ""
 *
 * @param stripFrom: string to strip spaces from
 * @param AdjustMem: if true, adjusts array sizes or deletes if entirely blank
 *
 * @return: length of stripped string
 */
int StripLeadingBlanks(char *stripFrom, bool AdjustMem = false)
{
  // assert ptr!=NULL
  int len = strlen(stripFrom);
  // find index of first not blank char
  int i = 0;
  while((i<len) && stripFrom[i] == ' ')
  { i++; }
  if(i==len-1) // entire string is blank
  { 
    stripFrom[0] = '\0';
    return strlen(stripFrom);
  }
  if(i == 0) // no leading spaces
  { return strlen(stripFrom); }
  int oldi = i;
  for(i; i<len; i++)
  { stripFrom[i-oldi] = stripFrom[i]; }
  stripFrom[i-oldi] = '\0';
  return strlen(stripFrom);
}

/*
 * StripTrailingBlanks
 * strips trailing blanks from a string. if string is entirely blank, string is set to ""
 *
 * @param stripFrom: string to strip spaces from
 * @param AdjustMem: if true, adjusts array sizes or deletes if entirely blank
 *
 * @return: length of stripped string
 */
int StripTrailingBlanks(char *stripFrom)
{
  // assert ptr!=NULL
  int len = strlen(stripFrom);
  int i = len-1;
  while((i>=0) && stripFrom[i] == ' ')
  { i--; }
  if(i == len) // no trailing blanks
  { return strlen(stripFrom); }
  if(i == 0) // entirely blank
  {
    stripFrom[0] = '\0';
    return strlen(stripFrom);
  }
  stripFrom[i+1] = '\0';
  return strlen(stripFrom);
}

/*
 * IsStringBlank
 * checks if a string contains only spaces
 *
 * @param str: string to check
 * 
 * @return: true if string is blank
 */
bool IsStringBlank(char *str)
{
  // assert ptr!=null
  int len = strlen(str);
  int i = 0;
  while((i<len) && str[i] == ' ')
  { i++; }
  if(i == len)
  { return true; }
  return false;
}

/* 
 * CountDataSets
 * counts the number of datasets in a csv like string
 *
 * @param cnt: csv string to count datasets of
 * @param delim: delimiter the datasets are seperated by
 * @leaveemptyout: if two delimiters follow each other its not counted as empty dataset
 *
 * @return: number of datasets
 */
int CountDataSets(char *cnt, char delim, bool leaveemptyout = false)
{
  int dscnt = 0;
  if(!leaveemptyout)
  {
    while (*cnt != '\0')
    {
      if(*cnt == delim) dscnt++;
      cnt++;
    }
    return dscnt+1;
  }
  else
  {
    int ln = strlen(cnt), i=0;
    while(i<ln)
    {
      if((cnt[i] == delim) && (i==0)) { i++; continue; } // delim an erster stelle bedeutet, daß wir als erstes einen leeren satz haben
      else if(((cnt[i] == delim) && (cnt[i-1] == delim)) && (i>0 || i<ln-1)) { i++; continue; } // wenn wir delim finden und das zeichen vorher auch delim ist, dann wieder leerer satz
      else if(((cnt[i] == delim) && (cnt[i-1] == delim)) && (i==ln-1)) { i++; continue; }
      else if(cnt[i] == delim) { dscnt++; i++; continue; }
      else i++;
    }
    if((dscnt == 0) && (ln > 0)) { dscnt++; }
    return dscnt+1; // richtig??
  }
}

/*
 * SplitString
 * saves substrings divided by a special char in an array
 * empty fields will be left out
 * 
 * @param toSplit: string to split
 * @param arr: array the substrings will be saved in, just give a char** ptr
 * @param splitAt: special char
 *
 * @return: number of fields
 */
char** SplitString(char *toSplit, char splitAt, int *cnt)
{
  char **arr;
  int numEl = CountDataSets(toSplit, ';', true);
  arr = new char*[numEl];
  // concept:
  // find length of next dataset and allocate mem for it, then copy dataset to arr and proceed
  int arridx = 0;
  while(*toSplit != '\0')
  {
    char bf[1024];
    int bfidx = 0;
    while((*toSplit != splitAt) && (*toSplit != '\0'))
    { bf[bfidx] = *toSplit; bfidx++; toSplit++; }
    if(bfidx == 0) { toSplit++; continue; }
    else
    {
      bf[bfidx] = '\0';
      int nl = strlen(bf);
      arr[arridx] = new char[nl+1];
      strcpy(arr[arridx], bf);
      arridx++;
    }
  }
  *cnt = numEl;
  return arr;
}

/*
 * GetMaskPart
 * parses a mask string in format *.txt;*.bmp;*.sfv and allows you to get a specific part of that "dataset"
 *
 * @param nIdx: index you want to get
 * @param mask: mask to process
 * @param buf: buffer the requested part is copied into
 * 
 * @return: part count if nIdx == -1 or value bigger than 1 if everything went ok
 */
int GetMaskPart(int nIdx, char *mask,  char *buf)
{
  int cnt = 0;
  char **arr = NULL;
  if((nIdx > -1) && ((buf == NULL)||(mask == NULL))) return 0;
  if((nIdx == -1) && mask != NULL) { SplitString(mask, ';', &cnt); return cnt; }
  arr = SplitString(mask,';', &cnt);
  if(nIdx < 0)
  { return cnt; }
  strcpy(buf, arr[nIdx]);
  return cnt;
}

#endif // __STRING_HELPER_H__