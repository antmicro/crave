set(CRAVE_LIB_SRC
    ConstrainedRandom.cpp
    ConstrainedRandomInit.cpp
    EvalVisitor.cpp
    FixWidthVisitor.cpp
    GetSupportSetVisitor.cpp
    metaSMTNodeVisitor.cpp
    metaSMTNodeVisitorYices2.cpp
    ReplaceVisitor.cpp
    ToDotNodeVisitor.cpp
    Generator.cpp
    VariableCoverageGenerator.cpp
    ConstraintPartition.cpp
    Evaluator.cpp
    RandObjGenerator.cpp
    VariableCoverageSolver.cpp
    VariableDefaultSolver.cpp
    VariableGeneratorType.cpp
    VariableSolver.cpp
    VectorGenerator.cpp
    ConstrainedRandom.cpp
    UserVectorConstraint.cpp
    UserConstraintType.cpp
    ConstraintManager.cpp
    ConstraintPartitioner.cpp
    Distribution.cpp
    RandomBase.cpp
    VariableGeneratorMT.cpp
    ComplexityEstimationVisitor.cpp
    RandomSeedManager.cpp)

add_library(${PROJECT_NAME} STATIC ${CRAVE_LIB_SRC})

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/metaSMT/include>
          $<INSTALL_INTERFACE:include>)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                                 ${PROJECT_BINARY_DIR}/lib)

target_link_directories(
  ${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/metaSMT/lib>
  $<INSTALL_INTERFACE:lib>)

target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
set_target_properties(
  ${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE on
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
)

set(LINK_LIBRARIES metaSMT ${ALL_EXTERNAL_LIBS})

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib)
