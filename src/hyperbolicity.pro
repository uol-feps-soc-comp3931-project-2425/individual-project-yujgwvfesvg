#-------------------------------------------------
# Project Configuration
#-------------------------------------------------
QT += core gui widgets
TARGET = hyperbolicity
CONFIG += c++17


PROJECT_ROOT = $$PWD # Path where the .pro file is
INCLUDE_DIR = $$PROJECT_ROOT/include
SRC_DIR = $$PROJECT_ROOT/src


INCLUDEPATH += ../include

#-------------------------------------------------
# Source Files
#-------------------------------------------------

SOURCES += frontend.cpp \
           backend.cpp \
           GraphParser.cpp \
           Hyperbolicity.cpp \
           ShortestPath.cpp \
           2Factor.cpp \
           LogApproximation.cpp \
           main.cpp

#-------------------------------------------------
# Header Files 
#-------------------------------------------------

HEADERS += ../include/frontend.h \
           ../include/backend.h \
           ../include/GraphParser.h \
           ../include/Hyperbolicity.h \
           ../include/ShortestPath.h \
           ../include/2Factor.h \
           ../include/LogApproximation.h

#-------------------------------------------------
# Platform-Specific Settings
#-------------------------------------------------

# --- OpenMP for Linux (GCC/Clang) ---
linux-g++*|linux-clang* {
    message("Enabling OpenMP for Linux GCC/Clang")
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp  # Linker flag often needed too
    LIBS += -lgomp            # Link the GNU OpenMP runtime library
}

# --- OpenMP for Windows (MSVC) ---
win32-msvc* {
    message("Enabling OpenMP for Windows MSVC")
    QMAKE_CXXFLAGS += /openmp # MSVC uses a combined compile/link flag
    # QMAKE_LFLAGS += /openmp # Usually not needed separately for MSVC
}

# --- OpenMP for Windows (MinGW GCC) ---
win32-g++* {
     message("Enabling OpenMP for Windows MinGW")
     QMAKE_CXXFLAGS += -fopenmp
     QMAKE_LFLAGS += -fopenmp
     LIBS += -lgomp            # Link the GNU OpenMP runtime library (if using MinGW's libgomp)
}

# --- macOS-specific settings (Using Homebrew) ---
macx {
    message("Applying macOS Homebrew-specific settings for Qt and OpenMP")

    
    HOMEBREW_PREFIX = /opt/homebrew # Default for Apple Silicon
    exists(/usr/local/bin/brew) {   # Check if Homebrew exists at Intel location
        HOMEBREW_PREFIX = /usr/local # Default for Intel Macs
        message("Detected Intel Mac Homebrew prefix: $$HOMEBREW_PREFIX")
    } else {
        message("Using Apple Silicon Homebrew prefix: $$HOMEBREW_PREFIX")
    }

    
    !exists($$HOMEBREW_PREFIX/opt/libomp/lib/libomp.dylib) {
        warning("libomp not found at $$HOMEBREW_PREFIX/opt/libomp. Please run: brew install libomp")
    }
    
    QMAKE_CXXFLAGS += -Xpreprocessor -fopenmp -I$$HOMEBREW_PREFIX/opt/libomp/include
    LIBS += -L$$HOMEBREW_PREFIX/opt/libomp/lib -lomp

   
    QT_BREW_FRAMEWORK_PATH = $$HOMEBREW_PREFIX/opt/qt/lib # Adjust qt@5/qt@6 if needed
    !exists($$QT_BREW_FRAMEWORK_PATH) {
         warning("Qt Framework path not found at $$QT_BREW_FRAMEWORK_PATH. Ensure Qt is installed via Homebrew.")
    } else {
         QMAKE_LFLAGS += -F$$QT_BREW_FRAMEWORK_PATH
         LIBS += -framework QtCore \
                 -framework QtGui \
                 -framework QtWidgets
         
    }
}

