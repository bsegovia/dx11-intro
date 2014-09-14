CC=run_vs cl
LD=run_vs link
FLAGS=/GS /analyze- /W3 /Gy- /Zc:wchar_t /Zi /Gm- /O1 /Ob1 /fp:fast \
  /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" \
  /fp:except- /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /Oi- /MD /c

LIBS=d3d11.lib \
  d3dcompiler.lib \
  kernel32.lib \
  user32.lib \
  gdi32.lib \
  winspool.lib \
  comdlg32.lib \
  advapi32.lib \
  shell32.lib \
  ole32.lib \
  oleaut32.lib \
  uuid.lib \
  winmm.lib \
  odbc32.lib \
  odbccp32.lib

LDFLAGS=/MANIFEST /LTCG /NXCOMPAT /DYNAMICBASE $(LIBS) /DEBUG /MACHINE:X86 \
  /OPT:REF /SAFESEH /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /OPT:ICF /ERRORREPORT:PROMPT \
  /NOLOGO /TLBID:1 /ENTRY:WinMain

all: normal

.PHONY: minified normal

normal: normal.exe
minified: minified.exe

MINIFIED_OBJS=minified.obj
NORMAL_OBJS=normal.obj

normal.obj: roadtohell.shader.h
minified.obj: roadtohell.shader.h

%.shader.h: %.hlsl
	cpp $< > $@.pre.hlsl
	cat $@.pre.hlsl | sed 's/#.*//g' > $@.tmp
	./bin/shader_minifier.exe --hlsl $@.tmp -o $@.h.tmp
	cat $@.h.tmp | sed 's/const\ char\ \*\(.*\)_tmp/static const char \1[]/g' > $@
	rm $@.pre.hlsl $@.tmp $@.h.tmp

%.obj: %.cpp
	bash --init-file bash.rc -i -c "$(CC) $(FLAGS) $< /Fo:$@" |\
	sed 's/\(.*\)pp(\([0-9]*\))\ :\(.*\)/\1pp:\2:1:\3/g'|\
	sed 's/\(.*\)h(\([0-9]*\))\ :\(.*\)/\1h:\2:1:\3/g'

normal.exe: $(NORMAL_OBJS)
	bash --init-file bash.rc -i -c "$(LD) /OUT:normal.exe /PDB:intro.pdb $(LDFLAGS) $(NORMAL_OBJS)"

minified.exe: $(MINIFIED_OBJS)
	bash --init-file crinkler.rc -i -c \
	"./bin/crinkler.exe /OUT:minified.exe /HASHTRIES:500 /SUBSYSTEM:WINDOWS /COMPMODE:SLOW /ORDERTRIES:5000 /TRUNCATEFLOATS:16 /HASHSIZE:500 /PRINT:IMPORTS /ENTRY:winmain /PRINT:LABELS /REPORT:report.html /RANGE:winmm.dll /RANGE:d3d11.dll /RANGE:d3dcompiler_47.dll winmm.lib kernel32.lib d3dcompiler.lib user32.lib d3d11.lib $(MINIFIED_OBJS)"

clean:
	rm -rf *html normal.exe minified.exe *manifest *.pdb $(MINIFIED_OBJS) $(NORMAL_OBJS)

