#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <chrono>
#include <mutex>
#include <Windows.h>

namespace fs = std::filesystem;

std::mutex mtxCerr;
std::mutex mtxCout;

void processFile(const fs::path& filePath, int& sum, std::mutex& mtx)
{
    std::ifstream file(filePath);
    
    std::string fileContent;
    int number{ 0 };
    
    //Check and print
    if (std::getline(file, fileContent))
    {
        try 
        {
            number = std::stoi(fileContent);

            std::lock_guard<std::mutex> lock(mtxCout);
            std::cout << filePath.filename().string() << ": " << number << std::endl;
        }
        catch (const std::exception& e)
        {
            std::lock_guard<std::mutex> lock(mtxCerr);
            std::cerr << "Wrong content in the file: " << filePath.string() << " with: " << e.what() << std::endl;
            return;
        }
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtxCerr);
        std::cerr << "File " << filePath.string() << " is empty\n";
        return;
    }
    
    //modifing outer variable
    {
        std::lock_guard<std::mutex> lock(mtx);
        sum += number;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

bool chooseDefaultFolder(std::string& path)
{
    //path to executable
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    fs::path newPath = fs::path(buffer).parent_path() / "Test";

    path = newPath.string();
    if (!fs::exists(path))
    {
        std::cerr << "Unfortunatelly there was no \"Test\" folder found nearby\n";
        std::cin.get();
        return 0;
    }
    std::cout << "Default folder was chosen\n";
    return 1;
}

int main(int argc, char* argv[])
{
    //choose of the path
    std::string targetPath;

    if (argc >= 2)
    {
        targetPath = argv[1];
    } 
    else
    {
        std::cout << "Please enter the folder path\nor just press \"Enter\" for default folder\n";
        std::getline(std::cin, targetPath);
    }

    if (fs::is_directory(targetPath) && fs::exists(targetPath))
    {
        if (fs::is_empty(targetPath))
        { 
            std::cout << "Selected folder is empty\nTrying default folder\n\n";
            
            if (!chooseDefaultFolder(std::ref(targetPath)))
            {
                return 0;
            }
        }
    }
    else if (targetPath.empty())
    {
        if (!chooseDefaultFolder(std::ref(targetPath)))
        {
            return 0;
        }
    }
    else
    {
        std::cout << "Incorrect path\nTrying default folder\n\n";
        if (!chooseDefaultFolder(std::ref(targetPath)))
        {
            return 0;
        }
    }

    //multithreading part
    std::vector<std::thread> threads;
    
    int totalSum{ 0 };
    std::mutex mtxForSharedSum;

    for (const auto& entry : fs::directory_iterator(targetPath))
    {
        if (fs::is_regular_file(entry)) 
        {
            threads.emplace_back(processFile, entry.path(), std::ref(totalSum), std::ref(mtxForSharedSum));
        }
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "The sum is " << totalSum << std::endl;
    
    std::cin.get();
    return 0;
}