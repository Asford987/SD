cmake_minimum_required(VERSION 3.10)
project(ComparadorMPI C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Procurar arquivos fonte
file(GLOB MPI_SOURCES "src/mpi_*.c")
file(GLOB SEQ_SOURCES "src/seq_*.c")
list(REMOVE_ITEM SEQ_SOURCES ${MPI_SOURCES})

find_package(MPI REQUIRED)
add_executable(paralelo ${MPI_SOURCES})
set_target_properties(paralelo PROPERTIES LINKER_LANGUAGE C)
target_include_directories(paralelo PRIVATE ${MPI_INCLUDE_PATH})
target_link_libraries(paralelo MPI::MPI_C)

add_executable(sequencial ${SEQ_SOURCES})

add_custom_target(run_mpi
    COMMAND mpirun -np 10 $<TARGET_FILE:paralelo>
    DEPENDS paralelo
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Executando versão MPI com 10 processos")

add_custom_target(run_seq
    COMMAND $<TARGET_FILE:sequencial>
    DEPENDS sequencial
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Executando versão sequencial")