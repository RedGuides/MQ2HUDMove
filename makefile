!include "../global.mak"

ALL : "$(OUTDIR)\MQ2HUDMove.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2HUDMove.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2HUDMove.dll"
	-@erase "$(OUTDIR)\MQ2HUDMove.exp"
	-@erase "$(OUTDIR)\MQ2HUDMove.lib"
	-@erase "$(OUTDIR)\MQ2HUDMove.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2HUDMove.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2HUDMove.dll" /implib:"$(OUTDIR)\MQ2HUDMove.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2HUDMove.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2HUDMove.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2HUDMove.dep")
!INCLUDE "MQ2HUDMove.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2HUDMove.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2HUDMove.cpp

"$(INTDIR)\MQ2HUDMove.obj" : $(SOURCE) "$(INTDIR)"

