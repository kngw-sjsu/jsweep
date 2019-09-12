target_sources(jsweep
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/State.h
            ${CMAKE_CURRENT_SOURCE_DIR}/AvailableCharactersState.h
            ${CMAKE_CURRENT_SOURCE_DIR}/CharacterState.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/GameState.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/TimerState.cpp
)
