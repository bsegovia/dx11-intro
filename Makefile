CC=run_vs cl
LD=run_vs link
FLAGS=/GS /analyze- /W3 /Gy- /Zc:wchar_t /Zi /Gm- /O1 /Ob1 /fp:fast \
  /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" \
  /fp:except- /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /Oi /MD /c

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
  odbc32.lib \
  odbccp32.lib

LDFLAGS=/MANIFEST /LTCG /NXCOMPAT /DYNAMICBASE $(LIBS) /DEBUG /MACHINE:X86 \
  /OPT:REF /SAFESEH /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /OPT:ICF /ERRORREPORT:PROMPT \
  /NOLOGO /TLBID:1 /ENTRY:winmain

all: intro

.PHONY: intro

intro: intro.exe
small: small.exe

OBJS=window.obj

window.c: qjulia.h

qjulia.h: qjulia.c
	gcc -E qjulia.c > qjulia.preprocessed.c
	cat qjulia.preprocessed.c | sed 's/#.*//g' > qjulia.tmp
	./bin/shader_minifier.exe --hlsl qjulia.tmp -o qjulia.h.tmp
	cat qjulia.h.tmp | sed 's/const\ char\ \*qjulia_tmp/static const char qjulia[]/g' > qjulia.h
	rm qjulia.preprocessed.c qjulia.tmp qjulia.h.tmp

%.obj: %.c
	bash --init-file bash.rc -i -c "$(CC) $(FLAGS) $< /Fo:$@"

intro.exe: $(OBJS)
	bash --init-file bash.rc -i -c "$(LD) /OUT:intro.exe /PDB:intro.pdb $(LDFLAGS) $(OBJS)"

small.exe: $(OBJS)
	bash --init-file crinkler.rc -i -c "./bin/crinkler.exe /OUT:small.exe /HASHTRIES:500 /SUBSYSTEM:WINDOWS /COMPMODE:SLOW /ORDERTRIES:5000 /TRUNCATEFLOATS:16 /HASHSIZE:500 /PRINT:IMPORTS /ENTRY:winmain /PRINT:LABELS /REPORT:report.html /RANGE:d3d11.dll /RANGE:d3dcompiler_47.dll kernel32.lib d3dcompiler.lib user32.lib d3d11.lib $(OBJS)"

clean:
	rm -rf *html intro.exe small.exe *.pdb $(OBJS)

