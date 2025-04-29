#-------------------------------------------------
# Project Configuration
#-------------------------------------------------
QT += core gui widgets
TARGET = hyperbolicity
CONFIG += c++17

# Define paths relative to the .pro file location
# Assumes .pro is in the root, and include/src are subdirs
# Adjust if your .pro file is elsewhere (e.g., inside src/)
PROJECT_ROOT = $$PWD # Path where the .pro file is
INCLUDE_DIR = $$PROJECT_ROOT/include
SRC_DIR = $$PROJECT_ROOT/src

# Tell the compiler where to find headers
INCLUDEPATH += ../include

#-------------------------------------------------
# Source Files
#-------------------------------------------------
# Using absolute paths based on SRC_DIR for clarity
SOURCES += frontend.cpp \
           backend.cpp \
           GraphParser.cpp \
           Hyperbolicity.cpp \
           ShortestPath.cpp \
           2Factor.cpp \
           LogApproximation.cpp \
           main.cpp

#-------------------------------------------------
# Header Files (qmake finds these via INCLUDEPATH)
#-------------------------------------------------
# Listing them helps Qt Creator/IDE but isn't strictly needed for compilation
# if they are included correctly in .cpp files and INCLUDEPATH is set.
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

    # Define potential Homebrew prefix locations (Apple Silicon first, then Intel)
    # Requires user to have Homebrew installed!
    HOMEBREW_PREFIX = /opt/homebrew # Default for Apple Silicon
    exists(/usr/local/bin/brew) {   # Check if Homebrew exists at Intel location
        HOMEBREW_PREFIX = /usr/local # Default for Intel Macs
        message("Detected Intel Mac Homebrew prefix: $$HOMEBREW_PREFIX")
    } else {
        message("Using Apple Silicon Homebrew prefix: $$HOMEBREW_PREFIX")
    }

    # Check for required Homebrew packages (optional but helpful diagnostic)
    !exists($$HOMEBREW_PREFIX/opt/libomp/lib/libomp.dylib) {
        warning("libomp not found at $$HOMEBREW_PREFIX/opt/libomp. Please run: brew install libomp")
    }
    # You could add a similar check for Qt if desired

    # OpenMP flags using Homebrew libomp
    QMAKE_CXXFLAGS += -Xpreprocessor -fopenmp -I$$HOMEBREW_PREFIX/opt/libomp/include
    LIBS += -L$$HOMEBREW_PREFIX/opt/libomp/lib -lomp

    # Qt framework linking using Homebrew Qt installation path
    QT_BREW_FRAMEWORK_PATH = $$HOMEBREW_PREFIX/opt/qt/lib # Adjust qt@5/qt@6 if needed
    !exists($$QT_BREW_FRAMEWORK_PATH) {
         warning("Qt Framework path not found at $$QT_BREW_FRAMEWORK_PATH. Ensure Qt is installed via Homebrew.")
    } else {
         QMAKE_LFLAGS += -F$$QT_BREW_FRAMEWORK_PATH
         LIBS += -framework QtCore \
                 -framework QtGui \
                 -framework QtWidgets
         # You might not need the LIBS += lines if -F correctly finds them,
         # but it can be more explicit. Test what works.
    }
}

