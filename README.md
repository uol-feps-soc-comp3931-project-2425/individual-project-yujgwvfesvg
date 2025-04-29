# Graph Hyperbolicity Calculator

A C++/Qt application designed to calculate the hyperbolicity of graphs. It features a graphical user interface (GUI) for loading graph datasets, executing calculations, and viewing the results. The project implements several graph algorithms relevant to hyperbolicity analysis.
![1main](https://github.com/user-attachments/assets/616960aa-3e69-4892-82b9-b54a53e93e1e)


## Features

* **Graphical User Interface:** Built with Qt Widgets for user interaction.
* **Graph Parsing:** Loads graph data from files (specify formats if known, e.g., edge lists, adjacency lists).
* **Hyperbolicity Calculation:** Core functionality to compute graph hyperbolicity delta values via 3 different methods.
* **Cross-Platform:** Designed to build and run on macOS, Linux, and Windows (with correct dependencies).
* **Parallelism:** Utilizes OpenMP for potential performance improvements in calculations.
* **Modern C++:** Written using C++17 standards.

## Prerequisites

Before building, ensure you have the following installed:

* **Git:** For cloning the repository.
* **Make:** A build automation tool (usually included with development toolchains).
* **C++17 Compliant Compiler:** A C++ compiler is a prerequisite as our functionality is implemented in the C++ language.
* **Qt Framework (Qt 5 or Qt 6 recommended):** Requires Core, GUI, and Widgets modules.
    * **macOS:** **Crucial:** Install using **Homebrew**: `brew install qt` (latest), `qt@6`, or `qt@5`. The project file relies on Homebrew paths.
    * **Linux (Debian/Ubuntu):** `sudo apt update && sudo apt install build-essential qtbase5-dev qttools5-dev` (or `qt6-base-dev`, etc.).
    * **Linux (Fedora):** `sudo dnf groupinstall "Development Tools" && sudo dnf install qt5-qtbase-devel qt5-qtwindowsystem-devel` (or `qt6` equivalents).
    * **Windows:** Use the official [Qt Online Installer](https://www.qt.io/download). Select a recent Qt version and ensure the `MSVC` or `MinGW` component (matching your compiler) includes `QtCore`, `QtGui`, and `QtWidgets`. Add `qmake` to your system's PATH.
* **OpenMP Runtime Library:** For parallel processing support.
    * **macOS:** **Crucial:** Install `libomp` using **Homebrew**: `brew install libomp`. The project file relies on Homebrew paths.
    * **Linux (Debian/Ubuntu):** `sudo apt install libomp-dev`.
    * **Linux (Fedora):** Usually provided by `libgomp` with the compiler toolchain.
    * **Windows:** Included with Visual Studio (enabled via `/openmp` flag in `.pro`). Included with most recent MinGW-w64 distributions (enabled via `-fopenmp` flag in `.pro`).

## Building the Project

1.  **Clone the repository:**
    ```bash
    git clone the repository
    cd [repository folder name]
    ```

2.  **Navigate to the source directory:**
    The qmake project file (`.pro`) is located inside the `src` directory.
    ```bash
    cd src
    ```

3.  **Clean previous build (optional but recommended):**
    ```bash
    make clean
    ```
    *(Ignore errors if no Makefile exists yet)*

4.  **Generate the Makefile:**
    Run `qmake` from within the `src` directory. It uses `hyperbolicity.pro` to create the platform-specific Makefile.
    ```bash
    # Ensure qmake from your installed Qt version is in your PATH
    qmake hyperbolicity.pro
    # Or potentially just: qmake
    ```
    *If qmake fails, verify Qt installation and PATH configuration.*

5.  **Compile the code:**
    Run `make`. This compiles the source files and links the executable. The `.pro` file is configured to enable OpenMP automatically.
    ```bash
    make
    # For faster compilation on multi-core systems, e.g., 4 cores:
    # make -j4
    ```
    *The compiled application will likely be created inside the `src` directory.*

## Running the Application

Navigate to the `src` directory (if not already there) where the application was built.

* **macOS:**
    An application bundle named `hyperbolicity.app` will be created. Launch it from the terminal:
    ```bash
    # Make sure you are in the src directory
    open ./hyperbolicity.app
    ```
    *You can also double-click the `.app` bundle in Finder.*

* **Linux:**
    An executable named `hyperbolicity` will be created. Launch it from the terminal:
    ```bash
    # Make sure you are in the src directory
    ./hyperbolicity
    ```
   

* **Windows:**
    An executable named `hyperbolicity.exe` will be created (potentially inside a `debug` or `release` subfolder within `src`, depending on your Qt/compiler setup). Launch it from the command prompt or PowerShell:
    ```bash
    # Make sure you are in the src directory (or src/debug, src/release)
    .\hyperbolicity.exe
    ```
    *You can also double-click the `.exe` file in File Explorer.*

## Usage

1.  Launch the application using the platform-specific instructions above.
2.  Use the GUI menu options (e.g., File > Open) to load a graph data file. Supported formats likely include [mention formats like edge lists (.txt), etc., if you know them]. Example datasets can be found in the `validation datasets/` and `real world datasets/` directories.
3.  Utilize the interface elements (buttons, input fields) to trigger hyperbolicity calculations and view the results.

Below is a screenshot of an input graph with hyperbolicity computation using two different methods.
![2main](https://github.com/user-attachments/assets/269fe7f1-1799-4366-87d8-30492cab8bd5)


## Project Structure
The project consists of the core implementations of the algoithms. On top of the core implementations, we have 3 datasets included within the project that can be uploaded directly to our application for computation. 
1.   Validation Dataset: These are the CSV files that make up the validation test for the project.
2.   Real World Dataset: These are the CSV representations of two real worls networks, namely West Yorkshire Bus Network, and Bitcoin Transaction Network. It also includes the python scripts that were used to compile these datasets.
3.   Evaluation Dataset: These are the CSV files representing the graphs of various sizes (vertices) that we used to evaluate the performance of the different algorithms.
