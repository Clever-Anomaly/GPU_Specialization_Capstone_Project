# Edge Detection NPP - GPU-Accelerated Edge Detection

**Fast, GPU-accelerated Sobel edge detection using NVIDIA NPP (NVIDIA Performance Primitives)**

Supports **JPG, PNG, BMP, PGM** and automatically converts color images to grayscale!

**Linux-only version**

---

## Demo and Validation

For a complete demonstration of this project and validation of its functionality, please refer to the **`code_run.ipynb`** notebook located in the root directory. This Jupyter notebook was executed on Google Colab and provides:

- Step-by-step execution of the edge detection pipeline
- Visual comparisons of input and output images
- Performance metrics and validation results
- Example use cases with different image types

The notebook serves as both documentation and proof of concept for the edge detection implementation.

---

## Prerequisites

Before building and running this project, ensure you have the following installed:

### Required Software
- **NVIDIA GPU** with CUDA support (Compute Capability 1.0+)
- **CUDA Toolkit 11.0+** (tested with 11.0 and later)
  - Download: https://developer.nvidia.com/cuda-downloads
- **FreeImage library** for image loading/saving
  - Install: `sudo apt-get install libfreeimage-dev`

### Verify Installation
```bash
# Check CUDA installation
nvcc --version
nvidia-smi

# Check NPP libraries
ls /usr/local/cuda/lib64/libnpp*
```

### Environment Setup
```bash
# Add to ~/.bashrc or ~/.zshrc
export CUDA_PATH=/usr/local/cuda
export LD_LIBRARY_PATH=$CUDA_PATH/lib64:$LD_LIBRARY_PATH
export PATH=$CUDA_PATH/bin:$PATH
```

---

## Quick Start

**IMPORTANT:** You must be in the `edgeDetectionNPP` directory to build and run the code!

### Navigate to Project Directory
```bash
cd edgeDetectionNPP
```

### Build the Project
```bash
./build.sh
```

### Run Edge Detection
```bash
# Use default test image from input/ directory
./edgeDetectionNPP

# Use your own image (place it in input/ directory first)
./edgeDetectionNPP --input=myimage.jpg
```

### Output
Results are saved to the `output/` directory:
- `{filename}_edges_horizontal.png` - Detects vertical edges (horizontal gradient)
- `{filename}_edges_vertical.png` - Detects horizontal edges (vertical gradient)

**No conversion needed - PNG files can be viewed directly!**

---

## Project Structure

```
edgeDetectionNPP/
├── input/                      # Place your input images here
│   ├── Lena.pgm               # Default test image
│   ├── sloth.png              # Sample images
│   └── your-images...         # Add your own images here
├── output/                     # All edge detection results saved here
│   ├── {filename}_edges_horizontal.png
│   └── {filename}_edges_vertical.png
├── edgeDetectionNPP.cpp       # Main source code
├── build.sh                   # Build script
├── README.md                  # This file
└── QUICKSTART.md              # Quick reference card
```

---

## Usage Examples

### Basic Usage
```bash
# Make sure you're in the edgeDetectionNPP directory!
cd edgeDetectionNPP

# Run with default image
./edgeDetectionNPP

# Run with custom image (place in input/ folder first)
cp ~/Pictures/photo.jpg input/
./edgeDetectionNPP --input=photo.jpg
```

### Workflow Example
```bash
# 1. Navigate to project directory
cd edgeDetectionNPP

# 2. Copy your images to input directory
cp ~/Documents/*.jpg input/
cp ~/Pictures/*.png input/

# 3. Process each image
./edgeDetectionNPP --input=vacation.jpg
./edgeDetectionNPP --input=selfie.png
./edgeDetectionNPP --input=document.jpg

# 4. View results in output directory
ls -lh output/
# vacation_edges_horizontal.png
# vacation_edges_vertical.png
# selfie_edges_horizontal.png
# selfie_edges_vertical.png
# document_edges_horizontal.png
# document_edges_vertical.png
```

### Supported Image Formats
- **JPG/JPEG** - JPEG images (color or grayscale)
- **PNG** - Portable Network Graphics (color or grayscale)
- **BMP** - Bitmap images (color or grayscale)
- **PGM** - Portable Graymap (grayscale only)
- **TGA, TIFF, and more** - Any format supported by FreeImage

**Note:** Color images are automatically converted to grayscale during processing.

---

## What This Project Does

### Edge Detection Explained
Edge detection identifies boundaries and transitions in images where pixel intensity changes significantly. This is fundamental to:
- Object recognition
- Image segmentation
- Feature extraction
- Computer vision pipelines

### Sobel Operator
This project uses the **Sobel operator**, an industry-standard edge detection algorithm:
- **3x3 convolution kernel** for gradient computation
- **Horizontal filter** detects vertical edges (left-right intensity changes)
- **Vertical filter** detects horizontal edges (top-bottom intensity changes)
- **GPU-accelerated** using NVIDIA NPP for high performance

### Why GPU Acceleration?
- **Fast processing** - Processes large images in milliseconds
- **Parallel computation** - Each pixel computed simultaneously on GPU
- **NPP library** - Highly optimized NVIDIA routines
- **Real-time capable** - Suitable for video processing

---

## Technical Details

### Key Technologies
- **Language:** C++ with CUDA
- **Libraries:** 
  - NPP (NVIDIA Performance Primitives) - GPU-accelerated image processing
  - FreeImage - Cross-platform image loading/saving
  - CUDA Runtime - GPU memory management and execution
- **Algorithm:** Sobel edge detection with border replication
- **Input:** Any common image format (auto-converted to 8-bit grayscale)
- **Output:** PNG format (8-bit grayscale edge maps)

### Performance Characteristics
- **Memory:** ~3x image size (input + 2 edge maps) on GPU
- **Speed:** Typically < 10ms for 512x512 images on modern GPUs
- **Quality:** Industry-standard Sobel operator with NPP_BORDER_REPLICATE

### Code Highlights
```cpp
// Horizontal edge detection (detects vertical edges)
nppiFilterSobelHorizBorder_8u_C1R(...)

// Vertical edge detection (detects horizontal edges)  
nppiFilterSobelVertBorder_8u_C1R(...)
```

---

## Use Cases

### Computer Vision
- **Object boundary detection** - Find outlines of objects
- **Feature extraction** - Prepare data for machine learning
- **Image segmentation** - Separate regions in images
- **Pattern recognition** - Identify shapes and structures

### Real-World Applications
- **Quality Control** - Automated defect detection in manufacturing
- **Document Processing** - Find text boundaries, table borders
- **Medical Imaging** - Identify anatomical structures, tumors
- **Autonomous Vehicles** - Lane detection, obstacle identification
- **Security Systems** - Motion detection, intrusion detection
- **Robotics** - Object recognition, navigation

---

## Building from Source

### Build Command
```bash
cd edgeDetectionNPP
./build.sh
```

The build script:
1. Checks for CUDA installation
2. Compiles with `nvcc` (NVIDIA CUDA Compiler)
3. Links NPP and FreeImage libraries
4. Creates `edgeDetectionNPP` executable

### Manual Build
```bash
nvcc -O2 -o edgeDetectionNPP edgeDetectionNPP.cpp \
  -I../Common -I../Common/UtilNPP \
  -L/usr/local/cuda/lib64 -lnppif -lnppisu -lnppc -lcudart -lfreeimage
```

---

## Command-Line Options

```bash
./edgeDetectionNPP [OPTIONS]

OPTIONS:
  --input=FILENAME    Specify input image (must be in input/ directory)
                      Example: --input=photo.jpg
                      
  (no arguments)      Uses default test image from input/ directory
                      Searches for: Lena.pgm, sloth.png, grey-sloth.png, th.jpeg
```

---

## Tips and Best Practices

### Image Preparation
- Place all input images in the `input/` directory
- Any image format works (JPG, PNG, BMP, PGM)
- Color images are automatically converted to grayscale
- No manual preprocessing required

### Output Files
- Check the `output/` directory for results
- PNG format for easy viewing
- `_horizontal.png` shows **vertical edges** (vertical lines in image)
- `_vertical.png` shows **horizontal edges** (horizontal lines in image)

### Performance Tips
- Larger images take longer (processing time ∝ pixel count)
- GPU memory must accommodate 3x image size
- For batch processing, write a script to process multiple images

### Understanding the Results
- **Bright pixels** = Strong edge detected
- **Dark pixels** = No edge / smooth region
- **Horizontal output** = Detects vertical structures (walls, poles, etc.)
- **Vertical output** = Detects horizontal structures (floors, horizons, etc.)

---

## Troubleshooting

### Build Errors

**Error: `nvcc: command not found`**
```bash
# Add CUDA to PATH
export PATH=/usr/local/cuda/bin:$PATH
```

**Error: `cannot find -lnppif` or `npp*.lib not found`**
```bash
# Add CUDA libraries to LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
```

**Error: `FreeImage.h: No such file`**
```bash
# Install FreeImage development package
sudo apt-get install libfreeimage-dev

# Or on Fedora/RHEL
sudo dnf install freeimage-devel
```

### Runtime Errors

**Error: `Unable to open file: <filename>`**
- Make sure you're in the `edgeDetectionNPP` directory
- Verify the image is in the `input/` directory
- Check the filename spelling

**Error: `No CUDA-capable device detected`**
```bash
# Check GPU status
nvidia-smi

# Verify CUDA can see your GPU
deviceQuery  # From CUDA samples
```

**Error: `NPP error` or CUDA errors**
- Ensure GPU drivers are up to date
- Check GPU has sufficient memory
- Verify CUDA Toolkit version compatibility

---

## Additional Resources

### Documentation
- **NPP Documentation:** https://docs.nvidia.com/cuda/npp/
- **CUDA Programming Guide:** https://docs.nvidia.com/cuda/cuda-c-programming-guide/
- **FreeImage Documentation:** http://freeimage.sourceforge.net/documentation.html

### Learning Materials
- **Sobel Operator:** https://en.wikipedia.org/wiki/Sobel_operator
- **Edge Detection:** https://en.wikipedia.org/wiki/Edge_detection
- **Image Processing:** https://www.nvidia.com/en-us/gtc/ (NVIDIA GTC talks)

### Related Projects
- NVIDIA NPP Samples: https://github.com/NVIDIA/cuda-samples
- OpenCV Edge Detection: https://docs.opencv.org/
- Image Processing Tutorials: https://www.nvidia.com/en-us/deep-learning-ai/

---

## Project Information

### Version
Edge Detection NPP - Standalone Version 1.0

### Author
**Saurav Haldar**  
Roll No: 2022464  
IIIT Delhi

### License
This is a simplified, standalone version extracted from the larger NVIDIA NPP samples collection.

### Requirements Summary
- **Hardware:** NVIDIA GPU (Compute Capability 1.0+)
- **Software:** CUDA Toolkit 11.0+, FreeImage library
- **OS:** Linux
- **Compiler:** nvcc (NVIDIA CUDA Compiler)

### Key Features
- **Multiple Format Support** - JPG, PNG, BMP, PGM, and more  
- **GPU Acceleration** - Fast processing with NVIDIA NPP  
- **Auto Grayscale Conversion** - Handles color images automatically  
- **Organized I/O** - Separate input/ and output/ directories  
- **Simple Build** - One-command compilation  
- **Production Ready** - Robust error handling and validation  

---

## Getting Started Checklist

- [ ] **Verify Prerequisites** - CUDA Toolkit, NVIDIA GPU, FreeImage installed
- [ ] **Navigate to Project** - `cd edgeDetectionNPP`
- [ ] **Build the Project** - Run `./build.sh`
- [ ] **Add Input Images** - Copy images to `input/` directory
- [ ] **Run Edge Detection** - `./edgeDetectionNPP --input=yourimage.jpg`
- [ ] **View Results** - Check `output/` directory for PNG files

---

## Summary

Everything is set up and ready for edge detection. Remember:
1. **Always run from the `edgeDetectionNPP` directory**
2. **Place input images in `input/` folder**
3. **Find results in `output/` folder**
4. **PNG output files open in any image viewer**

For validation and demonstration, see the `code_run.ipynb` notebook in the root directory.

---

*For quick reference, see `QUICKSTART.md` for essential commands.*
