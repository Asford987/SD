cmake_minimum_required(VERSION 3.10)
project(PrimosMPI C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Buscar fontes
file(GLOB MPI_SOURCES "src/mpi_*.c")
file(GLOB SEQ_SOURCES "src/seq_*.c")

# Encontrar MPI
find_package(MPI REQUIRED)

# ----------- Executável sequencial -----------
add_executable(seq_primos ${SEQ_SOURCES})
target_compile_options(seq_primos PRIVATE -O2)
target_link_libraries(seq_primos m)

# ----------- Executáveis MPI -----------
foreach(mpi_file ${MPI_SOURCES})
    get_filename_component(name ${mpi_file} NAME_WE)
    add_executable(${name} ${mpi_file})
    target_compile_options(${name} PRIVATE -O2)
    target_include_directories(${name} PRIVATE ${MPI_INCLUDE_PATH})
    target_link_libraries(${name} MPI::MPI_C m)
endforeach()

# ----------- Targets personalizados de execução -----------

add_custom_target(run_seq
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/seq_primos 100000000
    DEPENDS seq_primos
    COMMENT "Executando versão sequencial")

add_custom_target(run_saltos
    COMMAND mpirun -np 8 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mpi_saltos 100000000
    DEPENDS mpi_saltos
    COMMENT "Executando versão MPI (saltos intercalados) com 8 processos")

add_custom_target(run_saco
    COMMAND mpirun -np 8 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mpi_saco 100000000
    DEPENDS mpi_saco
    COMMENT "Executando versão MPI (saco de tarefas) com 8 processos")
