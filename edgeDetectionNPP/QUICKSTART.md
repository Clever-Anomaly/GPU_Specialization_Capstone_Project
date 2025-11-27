# Edge Detection NPP - Quick Reference

**Linux-only version**

## 📋 Prerequisites
```bash
# Check CUDA installation
nvcc --version
nvidia-smi

# Check NPP libraries
ls /usr/local/cuda/lib64/libnpp*

# Install FreeImage
sudo apt-get install libfreeimage-dev

# Set CUDA path (add to ~/.bashrc)
export CUDA_PATH=/usr/local/cuda
export LD_LIBRARY_PATH=$CUDA_PATH/lib64:$LD_LIBRARY_PATH
export PATH=$CUDA_PATH/bin:$PATH
```

## ⚠️ Important
**You must be in the `edgeDetectionNPP` directory to run the code!**
```bash
cd edgeDetectionNPP
```

## ✨ Now Supports All Image Formats!
**JPG, PNG, BMP, PGM** - Use any format directly!  
**Color images** are automatically converted to grayscale!  
**PNG output** - No conversion needed!

## 📁 Directory Structure
- **input/** - Place all your images here
- **output/** - All results saved here automatically

## Build
```bash
./build.sh
```

## Run
```bash
# Make sure you're in edgeDetectionNPP directory first!
cd edgeDetectionNPP

# Use any image from input/ directory
./edgeDetectionNPP --input=photo.jpg    # JPG
./edgeDetectionNPP --input=image.png    # PNG  
./edgeDetectionNPP --input=pic.bmp      # BMP
./edgeDetectionNPP                      # Default test image
```

## Output Files (in output/ directory, PNG format)
- `{filename}_edges_horizontal.png` - Vertical edges
- `{filename}_edges_vertical.png` - Horizontal edges

**No conversion needed - just open the PNG files!**

## Examples
```bash
# First, navigate to project directory
cd edgeDetectionNPP

# Copy your images to input folder
cp ~/Pictures/vacation.jpg input/
cp ~/Documents/selfie.png input/

# Process images
./edgeDetectionNPP --input=vacation.jpg
./edgeDetectionNPP --input=selfie.png

# Check results in output folder
ls output/
```

## What It Does
- Detects edges using Sobel operator
- GPU-accelerated processing
- Separates horizontal and vertical edges
- Perfect for computer vision applications

See README.md for full documentation.
