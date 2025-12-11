add_library(msgbase
        Parsers.cpp
        Messages.cpp
        MessageBuilder.cpp
        SequenceManager.cpp
        MessageCache.cpp
        MessageParserFactory.cpp
        ModuleAParser.cpp
        ModuleBParser.cpp
)

target_include_directories(msgbase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(msg_demo main.cpp)
target_link_libraries(msg_demo msgbase)
