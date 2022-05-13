/* MQ2HUDMove.cpp : Defines the entry point for the DLL application.
//
// 2005-03-09 - ieatacid
//
// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.


Commands and usage:

--- These work with the currently loaded HUD ---
/hudmove <HUD name>                                   - move a single HUD
/hudmove <HUD name> delete                            - delete a single HUD
/hudmove <HUD name> <up|down|left|right> <#>          - move a single HUD a specified number of units in whatever direction
/hudmove list                                         - list all HUDs in the loaded HUD set and all sections in MQ2HUD.ini
/hudlist                                              - same as above
/hudcolor <HUD name> <preset color name>              - change HUD to preset color
/hudcolor list                                        - list preset colors
-- This works for sections of HUDs --
/hudmove <section name> <up|down|left|right> <#>      - move all HUDs within a specified section
/hudmove ;<comment tag name> <up|down|left|right> <#> - move all huds within a comment section

Comment Tags:
- You can split sections of the ini using custom comments
- Comment tags are defined with ";<x>" (no quotes)
- You must follow the format ";<x> CommentTagName" (must be one word for now)
- Regular comments are ignored

Syntax:
/hudmove ;NameOfCommentTag <up|down|left|right> <#>
(the semi colon tells it to move a comment section)

Example: "/hudmove ;section1 up 50" would move all entries below ";<x> section1" until it
         reaches either the next ";<x>" or the next ini [section].

-- example ini (only ManaRegen and BuffCount would be moved) -----------------------
[blah]
Level=3,339,649,0,255,255,${If[${Target.ID},${Target.Level} - ${Target.Class} - ${Target.Distance} - ${Target.LineOfSight},]}
Raid=3,1263,468,0,255,255,${If[${Raid.Members}>0,In Raid - ${Raid.Members},]}
TarID=3,172,337,0,255,0,ID: ${Target.ID}
;<x> section1
ManaRegen=3,272,272,0,255,0,MR: ${Me.ManaRegen}
BuffCount=3,272,367,0,255,0,Buffs: ${Me.CountBuffs}
;<x> section2
ER=3,172,342,0,255,0,ER:  ${Me.AltAbilityTimer[Eldritch Rune].TimeHMS}
MGB=3,172,357,0,255,0,MGB:  ${Me.AltAbilityTimer[Mass Group Buff].TimeHMS}
------------------------------------------------------------------------------------

************************************************************************************/

#include <mq/Plugin.h>

PreSetup("MQ2HUDMove");
PLUGIN_VERSION(2005.0309);

char IniName[MAX_STRING]={0};
char LastSection[MAX_STRING]={0};

void SetLast()
{
   char Temp[MAX_STRING] = { 0 };
   char Section[MAX_STRING] = { 0 };
   sprintf_s(Temp, "%s_%s", pLocalPC->Name, GetServerShortName());
   GetPrivateProfileString(Temp, "Last", "NULL", Section, MAX_STRING, IniName);
   if (!strcmp(Section, "NULL"))
      GetPrivateProfileString("MQ2HUD", "Last", "Elements", Section, MAX_STRING, INIFileName);

   if (char* pch = strrchr(Section, ','))
      strcpy_s(LastSection, pch + 1);
   else
      strcpy_s(LastSection, Section);
}

// List HUDs
void List()
{
   char Temp[MAX_STRING]={0};
   char Section[MAX_STRING]={0};
   char IniString[MAX_STRING]={0};
   char* szElements=0;
   int i=0;
   GetPrivateProfileString(NULL,NULL,NULL,IniString,MAX_STRING,IniName);
   WriteChatf("\at--\ax [sections] \atin MQ2HUD.ini --\ax");
   szElements=IniString;
   for (i=0; i==0 || (IniString[i-1]!=0 || IniString[i]!=0) ; i++)
   {
      if(IniString[i]==0)
      {
         strcpy_s(Temp,szElements);
         if(!strcmp(Temp,"MQ2HUD")) break;
         WriteChatColor(Temp, COLOR_DEFAULT);
         szElements = &IniString[i+1];
      }
   }
   SetLast();
   GetPrivateProfileString(LastSection,NULL,NULL,IniString,MAX_STRING,IniName);
   WriteChatf("\at-- Available HUDs in loaded section\ax [%s] --",LastSection);
   if (IniString[0] != 0)
   {
      szElements = IniString;
      for (i = 0; i == 0 || (IniString[i - 1] != 0 || IniString[i] != 0); i++)
      {
         if (IniString[i] == 0)
         {
            strcpy_s(Temp, szElements);
            WriteChatColor(Temp, COLOR_DEFAULT);
            szElements = &IniString[i + 1];
         }
      }
   }
   else
   {
      WriteChatColor("~None~", COLOR_DEFAULT);
   }
}

// Re-write the ini string with new coordinates
template <unsigned int _Size>
char* FormatString(char(&NewIniString)[_Size],char* IniString,char* Direction,char* Units)
{
   char Type[MAX_STRING]={0}; GetArg(Type,IniString,1,FALSE,FALSE,TRUE);
   char XPos[MAX_STRING]={0}; GetArg(XPos,IniString,2,FALSE,FALSE,TRUE);
   char YPos[MAX_STRING]={0}; GetArg(YPos,IniString,3,FALSE,FALSE,TRUE);
   char Text[MAX_STRING]={0};
   char* Rest=GetNextArg(IniString,3,TRUE);
   strcpy_s(Text,Rest);

   if(!strcmp(Direction,"right"))
   {
      int NewX=(int)atoi(XPos)+(int)atoi(Units);
      sprintf_s(NewIniString,"%s,%i,%s,%s",Type,NewX,YPos,Text);
   }
   else if(!strcmp(Direction,"left"))
   {
      int NewX=(int)atoi(XPos)-(int)atoi(Units);
      sprintf_s(NewIniString,"%s,%i,%s,%s",Type,NewX,YPos,Text);
   }
   else if(!strcmp(Direction,"up"))
   {
      int NewY=(int)atoi(YPos)-(int)atoi(Units);
      sprintf_s(NewIniString,"%s,%s,%i,%s",Type,XPos,NewY,Text);
   }
   else if(!strcmp(Direction,"down"))
   {
      int NewY=(int)atoi(YPos)+(int)atoi(Units);
      sprintf_s(NewIniString,"%s,%s,%i,%s",Type,XPos,NewY,Text);
   }
   return NewIniString;
}

// Move all HUDs
bool MoveAll(char* MoveSection, char* Direction, char* Units)
{
   bool error = false;
   if (!strlen(Direction)) {
      WriteChatf("\arError:\ax No Direction provided. Options are left, right, up, or down.");
      error = true;
   }
   if (!strlen(Units)) {
      WriteChatf("\arError:\ax No Distance provided. Please provide a number of pixels to move %s.", (error ? "" : Direction));
      error = true;
   }
   if (error)
      return true;

   char Temp[MAX_STRING]={0};
   char IniString[MAX_STRING]={0};
   char NewIniString[MAX_STRING]={0};
   char* szElements=0;
   int i=0;

   if(!strcmp(MoveSection,"all"))
   {
      SetLast();
      GetPrivateProfileString(LastSection,NULL,NULL,IniString,MAX_STRING,IniName);
      szElements=IniString;
      for(i=0; i==0 || (IniString[i-1]!=0 || IniString[i]!=0) ; i++)
      {
         if(IniString[i]==0)
         {
            GetPrivateProfileString(LastSection,szElements,"NULL",Temp,MAX_STRING,IniName);
            FormatString(NewIniString,Temp,Direction,Units);
            WritePrivateProfileString(LastSection,szElements,NewIniString,IniName);
            szElements=&IniString[i+1];
         }
      }
      return true;
   }
   else
      GetPrivateProfileString(MoveSection,NULL,"NULL",IniString,MAX_STRING,IniName);

   if(!strcmp(IniString,"NULL"))
      return false;

   szElements=IniString;
   for(i=0; i==0 || (IniString[i-1]!=0 || IniString[i]!=0) ; i++)
   {
      if(IniString[i]==0)
      {
         GetPrivateProfileString(MoveSection,szElements,"NULL",Temp,MAX_STRING,IniName);
         FormatString(NewIniString,Temp,Direction,Units);
         WritePrivateProfileString(MoveSection,szElements,NewIniString,IniName);
         szElements=&IniString[i+1];
      }
   }
   return true;
}

// Move a custom ";<x> CommentSection" of HUDs
void MoveSection(char* MoveSection, char* Direction, char* Units)
{
   char Temp[MAX_STRING]={0};
   char Key[MAX_STRING]={0};
   char Section[MAX_STRING]={0};
   char IniString[MAX_STRING]={0};
   char NewIniString[MAX_STRING]={0};
   char IniSection[MAX_STRING]={0};
   char szLine[10240]={0};
   char * Tok = 0;
   char * Next_Token = 0;
   Tok=strtok_s(MoveSection,";", &Next_Token);
   strcpy_s(MoveSection,MAX_STRING,Tok);

   if(MoveSection[0]==0 || Direction[0]==0 || Units[0]==0)
   {
      WriteChatf("\arUsage:\ax /hudmove ;<comment tag name> <up|down|left|right> <#>");
      return;
   }
   if(!strcmp(Direction,"up") || !strcmp(Direction,"down") ||
      !strcmp(Direction,"left") || !strcmp(Direction,"right"))
   {
      FILE *file = 0;
      errno_t err = fopen_s(&file, IniName, "rt");
      if(!err)
      {
         while(fgets(szLine,10240,file))
         {
            if(szLine[0]=='[')
            {
               strtok_s(szLine,"]", &Next_Token);
               strcpy_s(Section,&szLine[1]);
            }
            if(strstr(szLine,";<x>"))
            {
               strtok_s(szLine,"\n", &Next_Token);
               strcpy_s(IniSection,&szLine[5]);
               if(!strcmp(MoveSection,IniSection))
               {
                  while(fgets(szLine,10240,file))
                  {
                     if(strstr(szLine,";<x>") || szLine[0]=='[') break;
                     if(szLine[0]!=';')
                     {
                        strtok_s(szLine,"=", &Next_Token);
                        sprintf_s(Temp,"%s\n",szLine);
                        strcat_s(Key,Temp);
                     }
                  }
                  break;
               }
            }
         }
      }
      else
      {
         WriteChatf("\arError:\ax Problem opening file");
         return;
      }
      fclose(file);
      if(Key[0]==NULL)
      {
         WriteChatf("\arError:\ax No such comment tag");
         return;
      }
      Tok=strtok_s(Key,"\n", &Next_Token);
      while(Tok!=NULL)
      {
         sprintf_s(Key,"%s",Tok);
         GetPrivateProfileString(Section,Key,"Elements",IniString,MAX_STRING,IniName);
         FormatString(NewIniString,IniString,Direction,Units);
         WritePrivateProfileString(Section,Key,NewIniString,IniName);
         Tok=strtok_s(NULL,"\n", &Next_Token);
      }
   }
   else
      WriteChatf("\arError:\ax Invalid direction specified");
   return;
}

// /hudmove
void HUDMove(PSPAWNINFO pChar, char* szLine)
{
   char Name[MAX_STRING]={0}; GetArg(Name,szLine,1);
   char Option[MAX_STRING]={0}; GetArg(Option,szLine,2);
   char Units[MAX_STRING]={0}; GetArg(Units,szLine,3);
   char Section[MAX_STRING]={0};
   char IniString[MAX_STRING]={0};
   char Temp[MAX_STRING]={0};

   if(Name[0]==0)
   {
      WriteChatColor("HUDMove usage:",COLOR_DEFAULT);
      WriteChatColor("/hudmove list\n/hudlist (same as \"/hudmove list\")",USERCOLOR_GUILD);
      WriteChatColor("------ Single HUD elements ------");
      WriteChatColor("/hudmove <hud name>\n/hudmove <hud name> delete",USERCOLOR_GUILD);
      WriteChatColor("/hudmove <hud name> <up|down|left|right> <#>",USERCOLOR_GUILD);
      WriteChatColor("/hudcolor <hud name> <preset color name>\n/hudcolor list (lists colors)",USERCOLOR_GUILD);
      WriteChatColor("------ Entire HUD sections ------");
      WriteChatColor("/hudmove <section name> <up|down|left|right|> <#>",USERCOLOR_GUILD);
      WriteChatColor("/hudmove ;<comment tag name> <up|down|left|right> <#>",USERCOLOR_GUILD);
      return;
   }
   else if(!strcmp(Name,"list"))
   {
      List();
      return;
   }
   else if(strstr(Name,";"))
   {
      MoveSection(Name,Option,Units);
      return;
   }
   else if(!strcmp(Name,"all"))
   {
      if(!strcmp(Option,"up") || !strcmp(Option,"down") ||
         !strcmp(Option,"left") || !strcmp(Option,"right") && Units[0]!=NULL)
      {
         if(!MoveAll(Name,Option,Units))
            WriteChatf("\arError moving HUD\ax");
         return;
      }
   }
   else if(MoveAll(Name,Option,Units))
      return;

   SetLast();
   GetPrivateProfileString(LastSection,Name,"NULL",IniString,MAX_STRING,IniName);
   if(!strcmp(IniString,"NULL"))
   {
      WriteChatf("\arError:\ax No HUD named \"%s\"",Name);
      return;
   }
   if(!strcmp(Option,"delete"))
   {
      WriteChatf("HUD deleted: %s",Name);
      WritePrivateProfileString(Section,Name,NULL,IniName);
      return;
   }
   char Type[MAX_STRING]={0};
   char Text[MAX_STRING]={0};
   char NewIni[MAX_STRING]={0};
   char* Rest;
   GetArg(Type,IniString,1,FALSE,FALSE,TRUE);
   Rest=GetNextArg(IniString,3,TRUE);
   strcpy_s(Text,Rest);

   if(!strcmp(Option,"up") || !strcmp(Option,"down") ||
      !strcmp(Option,"left") || !strcmp(Option,"right") && Units[0]!=NULL)
   {
      FormatString(NewIni,IniString,Option,Units);
   }
   else
   {
      int NewX=((MQMouseInfo*)EQADDR_MOUSE)->X;
      int NewY=((MQMouseInfo*)EQADDR_MOUSE)->Y;
      sprintf_s(NewIni,"%s,%d,%d,%s",Type,NewX,NewY,Text);
   }
   WritePrivateProfileString(LastSection,Name,NewIni,IniName);
   return;
}

// Color a single HUD
void HUDColor(PSPAWNINFO pChar, char* szLine)
{
   char Name[MAX_STRING]={0}; GetArg(Name,szLine,1);
   char Color[MAX_STRING]={0}; GetArg(Color,szLine,2);
   char Section[MAX_STRING]={0};
   char IniString[MAX_STRING]={0};

   if(Name[0]==0)
   {
      WriteChatColor("HUDColor usage:",COLOR_DEFAULT);
      WriteChatColor("/hudcolor list (list of colors)\n/hudcolor <name> <color>",USERCOLOR_GUILD);
      return;
   }
   else if(Color[0]==0)
   {
      WriteChatf("\arError:\ax Must specify a valid color");
      return;
   }
   else if(!strcmp(Name,"list"))
   {
      WriteChatColor("Available HUD colors:",COLOR_DEFAULT);
      WriteChatColor("red",CONCOLOR_RED);
      WriteChatColor("yellow",CONCOLOR_YELLOW);
      WriteChatColor("green",CONCOLOR_GREEN);
      WriteChatColor("darkgreen",COLOR_DARKGREEN);
      WriteChatColor("blue",CONCOLOR_BLUE);
      WriteChatColor("lightblue",CONCOLOR_LIGHTBLUE);
      WriteChatColor("purple",COLOR_PURPLE);
      WriteChatColor("lightgrey",COLOR_LIGHTGREY);
      WriteChatColor("darkgrey",COLOR_DARKGREY);
   }
   else
   {
      SetLast();
      GetPrivateProfileString(LastSection,Name,"NULL",IniString,MAX_STRING,IniName);
      if(!strcmp(IniString,"NULL"))
      {
         WriteChatf("\arError:\ax No HUD named \"%s\"",Name);
         return;
      }

      int NewR, NewG, NewB;
      NewR = NewG = NewB = 0;

      if(!strcmp(Color,"red"))
         NewR=255;
      else if(!strcmp(Color,"yellow"))
      {
         NewR=255;
         NewG=255;
      }
      else if(!strcmp(Color,"green"))
         NewG=255;
      else if(!strcmp(Color,"darkgreen"))
         NewG=175;
      else if(!strcmp(Color,"blue"))
         NewB=255;
      else if(!strcmp(Color,"lightblue"))
      {
         NewG=255;
         NewB=255;
      }
      else if(!strcmp(Color,"purple"))
      {
         NewR=255;
         NewB=255;
      }
      else if(!strcmp(Color,"lightgrey"))
      {
         NewR=175;
         NewG=175;
         NewB=175;
      }
      else if(!strcmp(Color,"darkgrey"))
      {
         NewR=75;
         NewG=75;
         NewB=75;
      }
      else
      {
         WriteChatf("\arError:\ax No preset color named \"%s\"",Color);
         return;
      }
      char Type[MAX_STRING]={0};
      char XPos[MAX_STRING]={0};
      char YPos[MAX_STRING]={0};
      char Text[MAX_STRING]={0};
      char NewIni[MAX_STRING]={0};
      char* Rest;
      GetArg(Type,IniString,1,FALSE,FALSE,TRUE);
      GetArg(XPos,IniString,2,FALSE,FALSE,TRUE);
      GetArg(YPos,IniString,3,FALSE,FALSE,TRUE);

      Rest=GetNextArg(IniString,6,TRUE);
      strcpy_s(Text,Rest);
      sprintf_s(NewIni,"%s,%s,%s,%d,%d,%d,%s",Type,XPos,YPos,NewR,NewG,NewB,Text);
      WritePrivateProfileString(LastSection,Name,NewIni,IniName);
   }
   return;
}

void HUDList(PSPAWNINFO pChar, char* szLine)
{
   List();
}

PLUGIN_API void InitializePlugin()
{
   strcpy_s(IniName,gPathConfig);
   strcat_s(IniName,"\\MQ2HUD.ini");
   AddCommand("/hudmove",HUDMove);
   AddCommand("/hudcolor",HUDColor);
   AddCommand("/hudlist",HUDList);
}

PLUGIN_API void ShutdownPlugin()
{
   RemoveCommand("/hudmove");
   RemoveCommand("/hudcolor");
   RemoveCommand("/hudlist");
}
