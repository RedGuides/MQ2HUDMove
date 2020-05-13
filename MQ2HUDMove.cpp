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

#include "../MQ2Plugin.h"

char IniName[MAX_STRING] = { 0 };
char LastSection[MAX_STRING] = { 0 };

PreSetup("MQ2HUDMove");

void SetLast()
{
    char Temp[MAX_STRING] = { 0 };
    char Section[MAX_STRING] = { 0 };

    sprintf_s(Temp, "%s_%s", GetCharInfo()->Name, EQADDR_SERVERNAME);
    GetPrivateProfileString(Temp, "Last", "NULL", Section, MAX_STRING, IniName);

    if (!_stricmp(Section, "NULL"))
        GetPrivateProfileString("MQ2HUD", "Last", "Elements", Section, MAX_STRING, INIFileName);

    if (char* pch = strrchr(Section, ','))
        strcpy_s(LastSection, pch + 1);
    else
        strcpy_s(LastSection, Section);

    return;
}

// List HUDs
void List()
{
    char Temp[MAX_STRING] = { 0 };
    char Section[MAX_STRING] = { 0 };
    char IniString[MAX_STRING] = { 0 };
    char* szElements = 0;

    GetPrivateProfileString(NULL, NULL, NULL, IniString, MAX_STRING, IniName);
    szElements = IniString;

    WriteChatf("\at--\ax [sections] \atin MQ2HUD.ini --\ax");
    for (int i = 0; i == 0 || (IniString[i - 1] != 0 || IniString[i] != 0); i++)
    {
        if (IniString[i] == 0)
        {
            strcpy_s(Temp, szElements);
            if (!_stricmp(Temp, "MQ2HUD")) break;
            WriteChatColor(Temp, COLOR_DEFAULT);
            szElements = &IniString[i + 1];
        }
    }

    SetLast();
    GetPrivateProfileString(LastSection, NULL, NULL, IniString, MAX_STRING, IniName);

    WriteChatf("\at-- Available HUDs in loaded section\ax [%s] --", LastSection);
    if (strlen(IniString))
    {
        szElements = IniString;
        for (int i = 0; i == 0 || (IniString[i - 1] != 0 || IniString[i] != 0); i++)
        {
            if (IniString[i] == 0)
            {
                strcpy_s(Temp, szElements);
                WriteChatColor(Temp, COLOR_DEFAULT);
                szElements = &IniString[i + 1];
            }
        }
    }
    else {
        WriteChatColor("~None~", COLOR_DEFAULT);
    }
}

// Re-write the ini string with new coordinates
template <unsigned int _Size>char* FormatString(char(&NewIniString)[_Size], char* IniString, char* Direction, char* Units)
{
    char Type[MAX_STRING] = { 0 };
    char XPos[MAX_STRING] = { 0 };
    char YPos[MAX_STRING] = { 0 };

    GetArg(Type, IniString, 1, FALSE, FALSE, TRUE);
    GetArg(XPos, IniString, 2, FALSE, FALSE, TRUE);
    GetArg(YPos, IniString, 3, FALSE, FALSE, TRUE);

    char Text[MAX_STRING] = { 0 };

    char* Rest = GetNextArg(IniString, 3, TRUE);
    strcpy_s(Text, Rest);

    if (!_stricmp(Direction, "right"))
    {
        int NewX = (int)atoi(XPos) + (int)atoi(Units);
        sprintf_s(NewIniString, "%s,%i,%s,%s", Type, NewX, YPos, Text);
    }
    else if (!_stricmp(Direction, "left"))
    {
        int NewX = (int)atoi(XPos) - (int)atoi(Units);
        sprintf_s(NewIniString, "%s,%i,%s,%s", Type, NewX, YPos, Text);
    }
    else if (!_stricmp(Direction, "up"))
    {
        int NewY = (int)atoi(YPos) - (int)atoi(Units);
        sprintf_s(NewIniString, "%s,%s,%i,%s", Type, XPos, NewY, Text);
    }
    else if (!_stricmp(Direction, "down"))
    {
        int NewY = (int)atoi(YPos) + (int)atoi(Units);
        sprintf_s(NewIniString, "%s,%s,%i,%s", Type, XPos, NewY, Text);
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

    char Temp[MAX_STRING] = { 0 };
    char IniString[MAX_STRING] = { 0 };
    char NewIniString[MAX_STRING] = { 0 };
    char* szElements = 0;

    if (!_stricmp(MoveSection, "all"))
    {
        SetLast();
        GetPrivateProfileString(LastSection, NULL, NULL, IniString, MAX_STRING, IniName);
        szElements = IniString;

        for (int i = 0; i == 0 || (IniString[i - 1] != 0 || IniString[i] != 0); i++)
        {
            if (IniString[i] == 0)
            {
                GetPrivateProfileString(LastSection, szElements, "NULL", Temp, MAX_STRING, IniName);
                FormatString(NewIniString, Temp, Direction, Units);
                WritePrivateProfileString(LastSection, szElements, NewIniString, IniName);
                szElements = &IniString[i + 1];
            }
        }

        return true;
    }
    else {
        GetPrivateProfileString(MoveSection, NULL, "NULL", IniString, MAX_STRING, IniName);
    }

    if (!_stricmp(IniString, "NULL"))
        return false;

    szElements = IniString;

    for (int i = 0; i == 0 || (IniString[i - 1] != 0 || IniString[i] != 0); i++)
    {
        if (IniString[i] == 0)
        {
            GetPrivateProfileString(MoveSection, szElements, "NULL", Temp, MAX_STRING, IniName);
            FormatString(NewIniString, Temp, Direction, Units);
            WritePrivateProfileString(MoveSection, szElements, NewIniString, IniName);
            szElements = &IniString[i + 1];
        }
    }

    return true;
}

// Move a custom ";<x> CommentSection" of HUDs
void MoveSection(char* MoveSection, char* Direction, char* Units)
{
    char Temp[MAX_STRING] = { 0 };
    char Key[MAX_STRING] = { 0 };
    char Section[MAX_STRING] = { 0 };
    char IniString[MAX_STRING] = { 0 };
    char NewIniString[MAX_STRING] = { 0 };
    char IniSection[MAX_STRING] = { 0 };
    char szLine[10240] = { 0 };

    char* Tok = 0;
    char* Next_Token = 0;
    Tok = strtok_s(MoveSection, ";", &Next_Token);
    strcpy_s(MoveSection, MAX_STRING, Tok);

    if (MoveSection[0] == 0 || Direction[0] == 0 || Units[0] == 0)
    {
        WriteChatf("\arUsage:\ax /hudmove ;<comment tag name> <up|down|left|right> <#>");
        return;
    }

    if (!_stricmp(Direction, "up") || !_stricmp(Direction, "down") || !_stricmp(Direction, "left") || !_stricmp(Direction, "right"))
    {
        FILE* file = 0;
        errno_t err = fopen_s(&file, IniName, "rt");
        if (!err)
        {
            while (fgets(szLine, 10240, file))
            {
                if (szLine[0] == '[')
                {
                    strtok_s(szLine, "]", &Next_Token);
                    strcpy_s(Section, &szLine[1]);
                }

                if (strstr(szLine, ";<x>"))
                {
                    strtok_s(szLine, "\n", &Next_Token);
                    strcpy_s(IniSection, &szLine[5]);

                    if (!_stricmp(MoveSection, IniSection))
                    {
                        while (fgets(szLine, 10240, file))
                        {
                            if (strstr(szLine, ";<x>") || szLine[0] == '[')
                                break;

                            if (szLine[0] != ';')
                            {
                                strtok_s(szLine, "=", &Next_Token);
                                sprintf_s(Temp, "%s\n", szLine);
                                strcat_s(Key, Temp);
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
        if (Key[0] == NULL)
        {
            WriteChatf("\arError:\ax No such comment tag");
            return;
        }

        Tok = strtok_s(Key, "\n", &Next_Token);

        while (Tok != NULL)
        {
            sprintf_s(Key, "%s", Tok);
            GetPrivateProfileString(Section, Key, "Elements", IniString, MAX_STRING, IniName);
            FormatString(NewIniString, IniString, Direction, Units);
            WritePrivateProfileString(Section, Key, NewIniString, IniName);
            Tok = strtok_s(NULL, "\n", &Next_Token);
        }
    }
    else {
        WriteChatf("\arError:\ax Invalid direction specified");
    }

    return;
}

//hudmove
void HUDMove(PSPAWNINFO pChar, char* szLine)
{
    char Name[MAX_STRING] = { 0 };
    char Option[MAX_STRING] = { 0 };
    char Units[MAX_STRING] = { 0 };

    GetArg(Name, szLine, 1);
    GetArg(Option, szLine, 2);
    GetArg(Units, szLine, 3);

    if (Name[0] == 0)
    {
        WriteChatColor("HUDMove usage:", COLOR_DEFAULT);
        WriteChatColor("/hudmove list\n/hudlist (same as \"/hudmove list\")", USERCOLOR_GUILD);
        WriteChatColor("------ Single HUD elements ------");
        WriteChatColor("/hudmove <hud name>\n/hudmove <hud name> delete", USERCOLOR_GUILD);
        WriteChatColor("/hudmove <hud name> <up|down|left|right> <#>", USERCOLOR_GUILD);
        WriteChatColor("/hudcolor <hud name> <preset color name>\n/hudcolor list (lists colors)", USERCOLOR_GUILD);
        WriteChatColor("------ Entire HUD sections ------");
        WriteChatColor("/hudmove <section name> <up|down|left|right|> <#>", USERCOLOR_GUILD);
        WriteChatColor("/hudmove ;<comment tag name> <up|down|left|right> <#>", USERCOLOR_GUILD);
        return;
    }

    if (!_stricmp(Name, "list"))
    {
        List();
        return;
    }

    if (strstr(Name, ";"))
    {
        MoveSection(Name, Option, Units);
        return;
    }
    else if (!_stricmp(Name, "all"))
    {
        if (!_stricmp(Option, "up") || !_stricmp(Option, "down") || !_stricmp(Option, "left") || !_stricmp(Option, "right") && Units[0] != NULL)
        {
            if (!MoveAll(Name, Option, Units))
                WriteChatf("\arError moving HUD\ax");

            return;
        }
    }
    else if (MoveAll(Name, Option, Units)) {
        return;
    }

    SetLast();
    char IniString[MAX_STRING] = { 0 };
    GetPrivateProfileString(LastSection, Name, "NULL", IniString, MAX_STRING, IniName);

    if (!_stricmp(IniString, "NULL"))
    {
        WriteChatf("\arError:\ax No HUD named \"%s\"", Name);
        return;
    }


    if (!_stricmp(Option, "delete"))
    {
        WriteChatf("HUD deleted: %s", Name);
        char Section[MAX_STRING] = { 0 };//This is never set to a value, so delete will either crash or delete the entire INI?
        WritePrivateProfileString(Section, Name, NULL, IniName);
        return;
    }

    char Type[MAX_STRING] = { 0 };
    char Text[MAX_STRING] = { 0 };
    char NewIni[MAX_STRING] = { 0 };
    char* Rest;
    GetArg(Type, IniString, 1, FALSE, FALSE, TRUE);
    Rest = GetNextArg(IniString, 3, TRUE);
    strcpy_s(Text, Rest);

    if (!_stricmp(Option, "up") || !_stricmp(Option, "down") || !_stricmp(Option, "left") || !_stricmp(Option, "right") && Units[0] != NULL)
    {
        FormatString(NewIni, IniString, Option, Units);
    }
    else
    {
        int NewX = ((PMOUSEINFO)EQADDR_MOUSE)->X;
        int NewY = ((PMOUSEINFO)EQADDR_MOUSE)->Y;
        sprintf_s(NewIni, "%s,%d,%d,%s", Type, NewX, NewY, Text);
    }

    WritePrivateProfileString(LastSection, Name, NewIni, IniName);
    return;
}

// Color a single HUD
void HUDColor(PSPAWNINFO pChar, char* szLine)
{
    char Name[MAX_STRING] = { 0 };
    GetArg(Name, szLine, 1);

    if (Name[0] == 0)
    {
        WriteChatColor("HUDColor usage:", COLOR_DEFAULT);
        WriteChatColor("/hudcolor list (list of colors)\n/hudcolor <name> <color>", USERCOLOR_GUILD);
        return;
    }


    char Color[MAX_STRING] = { 0 };
    GetArg(Color, szLine, 2);

    if (Color[0] == 0)
    {
        WriteChatf("\arError:\ax Must specify a valid color");
        return;
    }

    if (!_stricmp(Name, "list"))
    {
        WriteChatColor("Available HUD colors:", COLOR_DEFAULT);
        WriteChatColor("red", CONCOLOR_RED);
        WriteChatColor("yellow", CONCOLOR_YELLOW);
        WriteChatColor("green", CONCOLOR_GREEN);
        WriteChatColor("darkgreen", COLOR_DARKGREEN);
        WriteChatColor("blue", CONCOLOR_BLUE);
        WriteChatColor("lightblue", CONCOLOR_LIGHTBLUE);
        WriteChatColor("purple", COLOR_PURPLE);
        WriteChatColor("lightgrey", COLOR_LIGHTGREY);
        WriteChatColor("darkgrey", COLOR_DARKGREY);
        return;
    }

    SetLast();
    char IniString[MAX_STRING] = { 0 };
    GetPrivateProfileString(LastSection, Name, "NULL", IniString, MAX_STRING, IniName);

    if (!_stricmp(IniString, "NULL"))
    {
        WriteChatf("\arError:\ax No HUD named \"%s\"", Name);
        return;
    }

    int NewR = 0;
    int NewG = 0;
    int NewB = 0;

    if (!_stricmp(Color, "red"))
        NewR = 255;
    else if (!_stricmp(Color, "yellow"))
    {
        NewR = 255;
        NewG = 255;
    }
    else if (!_stricmp(Color, "green"))
        NewG = 255;
    else if (!_stricmp(Color, "darkgreen"))
        NewG = 175;
    else if (!_stricmp(Color, "blue"))
        NewB = 255;
    else if (!_stricmp(Color, "lightblue"))
    {
        NewG = 255;
        NewB = 255;
    }
    else if (!_stricmp(Color, "purple"))
    {
        NewR = 255;
        NewB = 255;
    }
    else if (!_stricmp(Color, "lightgrey"))
    {
        NewR = 175;
        NewG = 175;
        NewB = 175;
    }
    else if (!_stricmp(Color, "darkgrey"))
    {
        NewR = 75;
        NewG = 75;
        NewB = 75;
    }
    else
    {
        WriteChatf("\arError:\ax No preset color named \"%s\"", Color);
        return;
    }

    char Type[MAX_STRING] = { 0 };
    char XPos[MAX_STRING] = { 0 };
    char YPos[MAX_STRING] = { 0 };
    char* Rest;
    char Text[MAX_STRING] = { 0 };
    char NewIni[MAX_STRING] = { 0 };

    GetArg(Type, IniString, 1, FALSE, FALSE, TRUE);
    GetArg(XPos, IniString, 2, FALSE, FALSE, TRUE);
    GetArg(YPos, IniString, 3, FALSE, FALSE, TRUE);
    Rest = GetNextArg(IniString, 6, TRUE);
    strcpy_s(Text, Rest);
    sprintf_s(NewIni, "%s,%s,%s,%d,%d,%d,%s", Type, XPos, YPos, NewR, NewG, NewB, Text);

    WritePrivateProfileString(LastSection, Name, NewIni, IniName);
    return;
}

void HUDList(PSPAWNINFO pChar, char* szLine)
{
    List();
}

PLUGIN_API void InitializePlugin()
{
    strcpy_s(IniName, gszINIPath);
    strcat_s(IniName, "\\MQ2HUD.ini");
    AddCommand("/hudmove", HUDMove);
    AddCommand("/hudcolor", HUDColor);
    AddCommand("/hudlist", HUDList);
}

PLUGIN_API void ShutdownPlugin()
{
    RemoveCommand("/hudmove");
    RemoveCommand("/hudcolor");
    RemoveCommand("/hudlist");
}