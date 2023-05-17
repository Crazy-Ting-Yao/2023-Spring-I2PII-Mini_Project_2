cmake_minimum_required(VERSION 3.5)
project(2023_Spring_I2PII_Mini_Project_2)

set(CMAKE_CXX_STANDARD 14)

file (GLOB
        TD_SRC
        CONFIGURE_DEPENDS
        "src/*.cpp"
        "src/Bullet/*.cpp"
        "src/Effect/*.cpp"
        "src/Enemy/*.cpp"
        "src/Scene/*.cpp"
        "src/Turret/*.cpp"
)

add_executable(2023_Spring_I2PII_Mini_Project_2 ${TD_SRC} src/ToolsButton.cpp include/ToolsButton.hpp include/Shovel.hpp src/Shovel.cpp include/Shifter.hpp src/Shifter.cpp include/Enemy/DiceThreeEnemy.hpp include/Enemy/DiceFourEnemy.hpp include/Enemy/DiceFiveEnemy.hpp include/Enemy/DiceSixEnemy.hpp src/Enemy/DiceThreeEnemy.cpp src/Enemy/DiceFourEnemy.cpp src/Enemy/DiceFiveEnemy.cpp src/Enemy/DiceSixEnemy.cpp)
include_directories(${PROJECT_SOURCE_DIR})
include_directories("/usr/local/include")
link_directories("/usr/local/lib")

target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_acodec.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_audio.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_color.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_dialog.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_font.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_image.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_main.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_memfile.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_physfs.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_primitives.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_ttf.5.2.9.dylib)
target_link_libraries(2023_Spring_I2PII_Mini_Project_2 /usr/local/lib/liballegro_video.5.2.9.dylib)
