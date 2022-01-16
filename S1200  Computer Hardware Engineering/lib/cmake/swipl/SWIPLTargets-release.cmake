#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "swipl::swipl" for configuration "Release"
set_property(TARGET swipl::swipl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(swipl::swipl PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/SWI-Prolog.app/Contents/MacOS/swipl"
  )

list(APPEND _IMPORT_CHECK_TARGETS swipl::swipl )
list(APPEND _IMPORT_CHECK_FILES_FOR_swipl::swipl "${_IMPORT_PREFIX}/SWI-Prolog.app/Contents/MacOS/swipl" )

# Import target "swipl::libswipl" for configuration "Release"
set_property(TARGET swipl::libswipl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(swipl::libswipl PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/SWI-Prolog.app/Contents/Frameworks/libswipl.8.2.2.dylib"
  IMPORTED_SONAME_RELEASE "@rpath/libswipl.8.dylib"
  )

list(APPEND _IMPORT_CHECK_TARGETS swipl::libswipl )
list(APPEND _IMPORT_CHECK_FILES_FOR_swipl::libswipl "${_IMPORT_PREFIX}/SWI-Prolog.app/Contents/Frameworks/libswipl.8.2.2.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
