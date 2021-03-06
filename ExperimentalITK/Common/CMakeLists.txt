# Sources of non-templated classes.
SET(ITKCommon_SRCS
  itkGaborKernelFunction.cxx
)

IF (WIN32)
   SET(ITKCommon_SRCS ${ITKCommon_SRCS};itkWin32OutputWindow.cxx;itkSmartPointerForwardReferenceProcessObject.cxx)
ENDIF (WIN32)

# Instantiations not provided by vnl but used by ITK.
ADD_LIBRARY(itkvnl_inst
  vnl_c_vector+short-.cxx
  vnl_c_vector+ushort-.cxx
  vnl_c_vector+char-.cxx
  vnl_matrix+ushort-.cxx
  vnl_matrix+short-.cxx
  vnl_matrix+char-.cxx
  vnl_matrix_fixed+double.3.12-.cxx
  vnl_matrix_fixed_ref+double.2.2-.cxx
  vnl_matrix_fixed_ref+double.2.3-.cxx
  vnl_matrix_fixed_ref+double.3.12-.cxx
  vnl_matrix_fixed_ref+double.4.3-.cxx
  vnl_matrix_fixed_ref+double.4.4-.cxx
  vnl_vector+short-.cxx
  vnl_vector+ushort-.cxx
  vnl_vector+char-.cxx
  )
TARGET_LINK_LIBRARIES (itkvnl_inst ${VXL_NUMERICS_LIBRARIES} )

IF(ITK_EXPLICIT_INSTANTIATION)
  INCLUDE(${ITK_SOURCE_DIR}/Wrapping/ExplicitITK/Modules/Common/ExplicitITKCommon.cmake)
ENDIF(ITK_EXPLICIT_INSTANTIATION)

ADD_LIBRARY(ITKCommon ${ITK_COMMON_BUILD_TYPE} ${ITKCommon_SRCS} ${WRAPPER_EXPLICIT_ITKCommon_SRCS})

TARGET_LINK_LIBRARIES (ITKCommon itkvnl_inst itksys)
IF(UNIX)
  TARGET_LINK_LIBRARIES(ITKCommon ${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS} -lm)
ENDIF(UNIX)
IF(APPLE)
  TARGET_LINK_LIBRARIES(ITKCommon "-framework AppKit")
ENDIF(APPLE)

IF(ITK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(ITKCommon PROPERTIES ${ITK_LIBRARY_PROPERTIES})
  SET_TARGET_PROPERTIES(itkvnl_inst PROPERTIES ${ITK_LIBRARY_PROPERTIES})
ENDIF(ITK_LIBRARY_PROPERTIES)

IF(NOT ITK_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS ITKCommon itkvnl_inst
    RUNTIME DESTINATION ${ITK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT Development)
ENDIF(NOT ITK_INSTALL_NO_LIBRARIES)

IF(NOT ITK_INSTALL_NO_DEVELOPMENT)
  FILE(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  FILE(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
  INSTALL(FILES ${__files1} ${__files2}
    DESTINATION ${ITK_INSTALL_INCLUDE_DIR_CM24}/Common
    COMPONENT Development)
ENDIF(NOT ITK_INSTALL_NO_DEVELOPMENT)
