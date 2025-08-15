for %%i in (..\..\install\*.dll) do del "%%i"

del ..\..\install\python*.zip

copy ..\..\w32deps\openal\bin\OpenAL32.dll ..\..\install /Y
copy ..\..\w32deps\glew\bin\glew32.dll ..\..\install /Y
copy ..\..\w32deps\python\bin\python3*.dll ..\..\install /Y
copy ..\..\w32deps\python\bin\python3*.zip ..\..\install /Y

@rem Copy wxWidgets libraries (32 bit)

IF "%1" == "Debug" (SET WXLIB_SUFFIX=ud) ELSE (SET WXLIB_SUFFIX=u)

@echo Copying wxWidgets binaries

copy ..\..\w32deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_gl_vc*.dll	   ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_html_vc*.dll  ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_xrc_vc*.dll   ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_stc_vc*.dll   ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxbase32%WXLIB_SUFFIX%_vc*.dll	   ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxbase32%WXLIB_SUFFIX%_xml_vc*.dll  ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_adv_vc*.dll   ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_core_vc*.dll  ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_aui_vc*.dll  ..\..\install /Y
copy ..\..\w32deps\wxWidgets\bin\wxbase32%WXLIB_SUFFIX%_net_vc*.dll  ..\..\install /Y

IF "%1" == "Debug" (SET DEBUG_SUFFIX=-d) ELSE (SET DEBUG_SUFFIX=)

@echo Copying FTGL library (32 bit)
copy ..\..\w32deps\ftgl\bin\ftgl%DEBUG_SUFFIX%-%2.dll ..\..\install /Y

@echo Copying sigc++ library (32 bit)
copy "..\..\w32deps\libsigc++\bin\libsigc++%DEBUG_SUFFIX%-%2.dll" ..\..\install /Y

rem @echo Copying libgit2 library (32-bit)
rem copy ..\..\w32deps\libgit2\bin\libgit2%DEBUG_SUFFIX%.dll ..\..\install /Y