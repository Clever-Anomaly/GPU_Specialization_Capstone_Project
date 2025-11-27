/* Edge Detection NPP - Sobel edge detection using NPP
 * Supports JPG, PNG, BMP, PGM and other common image formats
 * Automatically converts color images to grayscale
 * Linux-only version
 */

#include <Exceptions.h>
#include <ImagesCPU.h>
#include <ImagesNPP.h>
#include <FreeImage.h>

#include <string.h>
#include <fstream>
#include <iostream>

#include <cuda_runtime.h>
#include <npp.h>

#include <helper_cuda.h>
#include <helper_string.h>

// Error handler for FreeImage
void FreeImageErrorHandler(FREE_IMAGE_FORMAT oFif, const char *zMessage)
{
    throw npp::Exception(zMessage);
}

// Enhanced image loader that handles color images automatically
void loadImageAutoGrayscale(const std::string &rFileName, npp::ImageCPU_8u_C1 &rImage)
{
    FreeImage_SetOutputMessage(FreeImageErrorHandler);

    // Detect image format
    FREE_IMAGE_FORMAT eFormat = FreeImage_GetFileType(rFileName.c_str());
    if (eFormat == FIF_UNKNOWN)
    {
        eFormat = FreeImage_GetFIFFromFilename(rFileName.c_str());
    }

    if (eFormat == FIF_UNKNOWN)
    {
        throw npp::Exception("Unknown image format");
    }

    // Load the image
    FIBITMAP *pBitmap = nullptr;
    if (FreeImage_FIFSupportsReading(eFormat))
    {
        pBitmap = FreeImage_Load(eFormat, rFileName.c_str());
    }

    if (!pBitmap)
    {
        throw npp::Exception("Failed to load image");
    }

    // Get image properties
    unsigned int bpp = FreeImage_GetBPP(pBitmap);
    FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(pBitmap);

    std::cout << "  Image format: " << FreeImage_GetFormatFromFIF(eFormat) << std::endl;
    std::cout << "  Image size: " << FreeImage_GetWidth(pBitmap) << "x" 
              << FreeImage_GetHeight(pBitmap) << std::endl;
    std::cout << "  Bits per pixel: " << bpp << std::endl;

    // Convert to 8-bit grayscale if needed
    FIBITMAP *pGrayBitmap = nullptr;
    if (colorType != FIC_MINISBLACK || bpp != 8)
    {
        std::cout << "  Converting to grayscale..." << std::endl;
        pGrayBitmap = FreeImage_ConvertToGreyscale(pBitmap);
        FreeImage_Unload(pBitmap);
        pBitmap = pGrayBitmap;
    }

    if (!pBitmap)
    {
        throw npp::Exception("Failed to convert to grayscale");
    }

    // Create image and copy data
    npp::ImageCPU_8u_C1 oImage(FreeImage_GetWidth(pBitmap), FreeImage_GetHeight(pBitmap));

    unsigned int nSrcPitch = FreeImage_GetPitch(pBitmap);
    const Npp8u *pSrcLine = FreeImage_GetBits(pBitmap) + nSrcPitch * (FreeImage_GetHeight(pBitmap) - 1);
    Npp8u *pDstLine = oImage.data();
    unsigned int nDstPitch = oImage.pitch();

    for (size_t iLine = 0; iLine < oImage.height(); ++iLine)
    {
        memcpy(pDstLine, pSrcLine, oImage.width() * sizeof(Npp8u));
        pSrcLine -= nSrcPitch;
        pDstLine += nDstPitch;
    }

    FreeImage_Unload(pBitmap);
    oImage.swap(rImage);
}

// Save grayscale image (supports PGM and PNG)
void saveImageGrayscale(const std::string &rFileName, const npp::ImageCPU_8u_C1 &rImage)
{
    FIBITMAP *pResultBitmap = FreeImage_Allocate(rImage.width(), rImage.height(), 8);
    if (!pResultBitmap)
    {
        throw npp::Exception("Failed to allocate result image");
    }

    unsigned int nDstPitch = FreeImage_GetPitch(pResultBitmap);
    Npp8u *pDstLine = FreeImage_GetBits(pResultBitmap) + nDstPitch * (rImage.height() - 1);
    const Npp8u *pSrcLine = rImage.data();
    unsigned int nSrcPitch = rImage.pitch();

    for (size_t iLine = 0; iLine < rImage.height(); ++iLine)
    {
        memcpy(pDstLine, pSrcLine, rImage.width() * sizeof(Npp8u));
        pSrcLine += nSrcPitch;
        pDstLine -= nDstPitch;
    }

    // Determine format from extension
    FREE_IMAGE_FORMAT format = FIF_PGM;
    std::string::size_type dot = rFileName.rfind('.');
    if (dot != std::string::npos)
    {
        std::string ext = rFileName.substr(dot + 1);
        if (ext == "png" || ext == "PNG")
            format = FIF_PNG;
        else if (ext == "bmp" || ext == "BMP")
            format = FIF_BMP;
        else if (ext == "jpg" || ext == "JPG" || ext == "jpeg" || ext == "JPEG")
            format = FIF_JPEG;
    }

    bool bSuccess = FreeImage_Save(format, pResultBitmap, rFileName.c_str(), 0) == TRUE;
    FreeImage_Unload(pResultBitmap);

    if (!bSuccess)
    {
        throw npp::Exception("Failed to save result image");
    }
}

bool printfNPPinfo(int argc, char *argv[])
{
    const NppLibraryVersion *libVer = nppGetLibVersion();

    printf("NPP Library Version %d.%d.%d\n", libVer->major, libVer->minor, libVer->build);

    int driverVersion, runtimeVersion;
    cudaDriverGetVersion(&driverVersion);
    cudaRuntimeGetVersion(&runtimeVersion);

    printf("  CUDA Driver  Version: %d.%d\n", driverVersion / 1000, (driverVersion % 100) / 10);
    printf("  CUDA Runtime Version: %d.%d\n", runtimeVersion / 1000, (runtimeVersion % 100) / 10);

    bool bVal = checkCudaCapabilities(1, 0);
    return bVal;
}

int main(int argc, char *argv[])
{
    printf("%s Starting...\n\n", argv[0]);
    printf("Supports: JPG, PNG, BMP, PGM and other image formats\n");
    printf("Automatically converts color images to grayscale\n\n");

    try
    {
        std::string sFilename;
        char *filePath;

        findCudaDevice(argc, (const char **)argv);

        if (printfNPPinfo(argc, argv) == false)
        {
            exit(EXIT_SUCCESS);
        }

        // Get input filename
        if (checkCmdLineFlag(argc, (const char **)argv, "input"))
        {
            getCmdLineArgumentString(argc, (const char **)argv, "input", &filePath);
            sFilename = std::string("input/") + filePath;
        }
        else
        {
            // Try common test images in input directory
            const char* testImages[] = {"input/Lena.pgm", "input/sloth.png", "input/grey-sloth.png", "input/th.jpeg"};
            bool found = false;
            for (int i = 0; i < 4; i++)
            {
                std::ifstream test(testImages[i]);
                if (test.good())
                {
                    sFilename = testImages[i];
                    found = true;
                    test.close();
                    break;
                }
                test.close();
            }
            if (!found)
            {
                sFilename = "input/Lena.pgm";
            }
        }

        // Check if file exists
        std::ifstream infile(sFilename.data(), std::ifstream::in);
        if (!infile.good())
        {
            std::cout << "Error: Unable to open file: <" << sFilename << ">" << std::endl;
            std::cout << "\nUsage: " << argv[0] << " --input=image.[jpg|png|bmp|pgm]" << std::endl;
            std::cout << "Note: Place your images in the 'input/' directory" << std::endl;
            std::cout << "\nExample: " << argv[0] << " --input=photo.jpg" << std::endl;
            std::cout << "         (File should be at: input/photo.jpg)" << std::endl;
            infile.close();
            exit(EXIT_FAILURE);
        }
        infile.close();

        std::cout << "Loading image: " << sFilename << std::endl;

        // Load image (automatically converts to grayscale)
        npp::ImageCPU_8u_C1 oHostSrc;
        loadImageAutoGrayscale(sFilename, oHostSrc);
        
        std::cout << "✓ Image loaded successfully\n" << std::endl;

        // Copy to GPU
        npp::ImageNPP_8u_C1 oDeviceSrc(oHostSrc);

        NppiSize oSrcSize = {(int)oDeviceSrc.width(), (int)oDeviceSrc.height()};
        NppiPoint oSrcOffset = {0, 0};
        NppiSize oSizeROI = {(int)oDeviceSrc.width(), (int)oDeviceSrc.height()};

        // Allocate device images for horizontal and vertical Sobel
        npp::ImageNPP_8u_C1 oDeviceDstH(oSizeROI.width, oSizeROI.height);
        npp::ImageNPP_8u_C1 oDeviceDstV(oSizeROI.width, oSizeROI.height);

        std::cout << "Applying Sobel edge detection..." << std::endl;

        // Apply Sobel horizontal edge detection
        std::cout << "  Computing horizontal edges..." << std::endl;
        NPP_CHECK_NPP(nppiFilterSobelHorizBorder_8u_C1R(
            oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
            oDeviceDstH.data(), oDeviceDstH.pitch(), oSizeROI,
            NPP_BORDER_REPLICATE));

        // Apply Sobel vertical edge detection
        std::cout << "  Computing vertical edges..." << std::endl;
        NPP_CHECK_NPP(nppiFilterSobelVertBorder_8u_C1R(
            oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
            oDeviceDstV.data(), oDeviceDstV.pitch(), oSizeROI,
            NPP_BORDER_REPLICATE));

        // Copy results back to host
        npp::ImageCPU_8u_C1 oHostDstH(oDeviceDstH.size());
        oDeviceDstH.copyTo(oHostDstH.data(), oHostDstH.pitch());

        npp::ImageCPU_8u_C1 oHostDstV(oDeviceDstV.size());
        oDeviceDstV.copyTo(oHostDstV.data(), oHostDstV.pitch());

        // Generate output filenames in output directory
        std::string sResultFilenameH = sFilename;
        std::string sResultFilenameV = sFilename;
        
        // Extract just the filename without path
        std::string::size_type lastSlash = sFilename.rfind('/');
        if (lastSlash == std::string::npos)
            lastSlash = sFilename.rfind('\\');
        
        std::string baseFilename;
        if (lastSlash != std::string::npos)
            baseFilename = sFilename.substr(lastSlash + 1);
        else
            baseFilename = sFilename;
        
        std::string::size_type dot = baseFilename.rfind('.');

        if (dot != std::string::npos)
        {
            baseFilename = baseFilename.substr(0, dot);
        }

        sResultFilenameH = "output/" + baseFilename + "_edges_horizontal.png";
        sResultFilenameV = "output/" + baseFilename + "_edges_vertical.png";

        // Save results as PNG for easy viewing
        std::cout << "\nSaving results..." << std::endl;
        saveImageGrayscale(sResultFilenameH, oHostDstH);
        std::cout << "  ✓ Saved: " << sResultFilenameH << std::endl;

        saveImageGrayscale(sResultFilenameV, oHostDstV);
        std::cout << "  ✓ Saved: " << sResultFilenameV << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "Edge detection complete!" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "\nOutput files:" << std::endl;
        std::cout << "  • Horizontal edges (vertical lines): " << sResultFilenameH << std::endl;
        std::cout << "  • Vertical edges (horizontal lines): " << sResultFilenameV << std::endl;
        std::cout << "\nTip: PNG files can be viewed directly in any image viewer!" << std::endl;

        // Free device memory
        nppiFree(oDeviceSrc.data());
        nppiFree(oDeviceDstH.data());
        nppiFree(oDeviceDstV.data());

        exit(EXIT_SUCCESS);
    }
    catch (npp::Exception &rException)
    {
        std::cerr << "\nProgram error! The following exception occurred: \n";
        std::cerr << rException << std::endl;
        std::cerr << "Aborting." << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "\nProgram error! An unknown exception occurred." << std::endl;
        std::cerr << "Aborting." << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
