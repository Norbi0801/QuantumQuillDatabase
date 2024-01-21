cmake_minimum_required(VERSION 3.27)
project(QuantumQuill)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.1.1
)

FetchContent_MakeAvailable(fmt)

add_executable(QuantumQuill main.cpp
        Core/Database.hpp
        Core/Engine.hpp
        Core/Reader.hpp
        Core/Logdb.hpp
        Core/Writer.hpp
        Core/QueryInterpreter.hpp
        Core/ClientInput.hpp
        Core/Database.cpp
        Core/Reader.cpp
        Core/Writer.cpp
        Core/Engine.cpp
        Core/ClientInput.cpp
        Core/Logdb.cpp
        Structures/Functions.cpp
        Commands/Command.hpp
        Commands/CreateTable.hpp
        Commands/DropTable.hpp
        Commands/AddColumn.hpp
        Commands/Select.hpp
        Commands/CreateRelation.hpp
        Commands/DropRelation.hpp
        Commands/AddRow.hpp
        Parsers/LogicalExpressions.hpp
        Commands/UpdateRow.hpp
        Structures/Types.hpp
        Commands/RemoveRow.hpp
        Parsers/SelectQuery.hpp
        Commands/Help.hpp
        Commands/LoadDatabase.hpp)

target_link_libraries(QuantumQuill PRIVATE fmt::fmt)