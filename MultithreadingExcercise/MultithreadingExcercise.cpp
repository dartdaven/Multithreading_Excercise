#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

int cointFilesInDirectory(const std::string& folderPath)
{
    int fileCount{ 0 };
    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (fs::is_regular_file(entry)) { fileCount++; }
    }
    return fileCount;
}

int main(int argc, char* argv[])
{
    std::string targetPath;

    if (argc >= 2)
    {
        targetPath = argv[1];
    } 
    else
    {
        std::cout << "Please enter the folder path\n";
        std::getline(std::cin, targetPath);
    }

    if (targetPath.empty() || !fs::exists(targetPath)) 
    { 
        targetPath = "Test";
        if (!fs::exists(targetPath))
        {
            std::cout << "Unfortunatelly there was no \"Test\" folder found nearby\n";
            std::cin.get();
            return 0;
        }
        std::cout << "Default folder was chosen\n";
    }

    int fileCount = cointFilesInDirectory(targetPath);

    std::cout << "there is " << fileCount << " files\n";
    std::cin.get();

    return 0;
}
