cmake_minimum_required(VERSION 3.29)
project(montecarloVaR)

set(CMAKE_CXX_STANDARD 20)

# Find pybind11: in the terminal run python -m pybind11 --cmakedir
set(pybind11_DIR "/Users/madararubino/anaconda3/envs/env-trading/lib/python3.8/site-packages/pybind11/share/cmake/pybind11")  # Manually set pybind11 directory

find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module Development.Embed)
find_package(pybind11 REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(montecarloVaR main.cpp
        Asset.h
        Equity.h
        Equity.cpp
        functions.h
        functions.cpp
        Random.h
        Portfolio.h
        Portfolio.cpp
        MultiEquityPortfolio.h
        MultiEquityPortfolio.cpp
)

# Link against Python3 and pybind11
target_link_libraries(montecarloVaR PRIVATE pybind11::embed Python3::Python)
