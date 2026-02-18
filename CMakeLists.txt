cmake_minimum_required(VERSION 3.16)

# 프로젝트 명 및 버전 설정
project(DCDCPowerAnalyzer VERSION 1.0.0 LANGUAGES CXX)

# C++ 표준 설정 (Qt 6.10이므로 최신 기능을 위해 C++20 추천)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 관련 자동 설정 (MOC, UI, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt 6.10 패키지 로드 (Widgets는 GUI용, Charts는 Bode Plot용)
find_package(Qt6 6.10 REQUIRED COMPONENTS Core Widgets Charts)

# 헤더 파일 경로 포함 (src 폴더를 기준으로 상대 경로 사용 가능하게 설정)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 소스 파일 목록 정의 (폴더별로 구분하여 관리)
set(SOURCES
    src/main.cpp
    
    # Core Logic
    src/core/ConverterBase.cpp
    src/core/BuckConverter.cpp
    src/core/BoostConverter.cpp

    # Models
    src/models/PowerStageParams.cpp
    src/models/AnalysisResult.cpp
    
    # Utils
    src/utils/MathUtils.cpp
    
    # UI (추후 MainWindow 추가 시 주석 해제)
    src/ui/MainWindow.cpp
)

# 헤더 파일 목록 (필요 시 명시, 보통 AUTOMOC이 처리함)
set(HEADERS
    src/core/ConverterBase.h
    src/core/BuckConverter.h
    src/core/BoostConverter.h

    src/models/PowerStageParams.h
    src/models/AnalysisResult.h

    src/utils/MathUtils.h
    
    src/ui/MainWindow.h
)

# 실행 파일 생성
add_executable(PowerStageAnalyzer ${SOURCES} ${HEADERS})

# 라이브러리 링크
target_link_libraries(PowerStageAnalyzer PRIVATE 
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
)

# 윈도우 앱 설정 (콘솔창 출력 여부 선택)
if(WIN32)
    set_target_properties(PowerStageAnalyzer PROPERTIES WIN32_EXECUTABLE TRUE)
    # set_target_properties(PowerStageAnalyzer PROPERTIES WIN32_EXECUTABLE FALSE)
endif()