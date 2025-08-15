for %%i in (..\..\install\*.dll) do del "%%i"

del ..\..\install\python*.zip

copy ..\..\w64deps\openal\bin\OpenAL32.dll ..\..\install /Y
copy ..\..\w64deps\glew\bin\glew32.dll ..\..\install /Y
copy ..\..\w64deps\python\bin\python3*.dll ..\..\install /Y
copy ..\..\w64deps\python\bin\python3*.zip ..\..\install /Y

@rem Copy wxWidgets libraries (64 bit)

IF "%1" == "Debug" (SET WXLIB_SUFFIX=ud) ELSE (SET WXLIB_SUFFIX=u)

@echo Copying wxWidgets binaries

copy ..\..\w64deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_gl_vc*_x64.dll	   ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_html_vc*_x64.dll  ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_xrc_vc*_x64.dll   ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_stc_vc*_x64.dll   ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxbase32%WXLIB_SUFFIX%_vc*_x64.dll	   ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxbase32%WXLIB_SUFFIX%_xml_vc*_x64.dll  ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_adv_vc*_x64.dll   ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_core_vc*_x64.dll  ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxmsw32%WXLIB_SUFFIX%_aui_vc*_x64.dll  ..\..\install /Y
copy ..\..\w64deps\wxWidgets\bin\wxbase32%WXLIB_SUFFIX%_net_vc*_x64.dll  ..\..\install /Y

IF "%1" == "Debug" (SET DEBUG_SUFFIX=-d) ELSE (SET DEBUG_SUFFIX=)

@echo Copying FTGL library (64 bit)
copy ..\..\w64deps\ftgl\bin\ftgl%DEBUG_SUFFIX%-%2.dll ..\..\install /Y

@echo Copying sigc++ library (64 bit)
copy "..\..\w64deps\libsigc++\bin\libsigc++%DEBUG_SUFFIX%-%2.dll" ..\..\install /Y

rem @echo Copying libgit2 library (64-bit)
rem copy ..\..\w64deps\libgit2\bin\libgit2%DEBUG_SUFFIX%.dll ..\..\install /Y