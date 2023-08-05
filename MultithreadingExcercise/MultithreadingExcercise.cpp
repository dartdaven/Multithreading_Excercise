#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <Windows.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>
#include <boost/bind/bind.hpp>

namespace fs = std::filesystem;

boost::mutex mtxCerr;
boost::mutex mtxCout;

void processFile(const fs::path& filePath, boost::atomic<int>& sum)
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

            boost::lock_guard<boost::mutex> lock(mtxCout);
            std::cout << filePath.filename().string() << ": " << number << std::endl;
        }
        catch (const std::exception& e)
        {
            boost::lock_guard<boost::mutex> lock(mtxCerr);
            std::cerr << "Wrong content in the file: " << filePath.string() << " with: " << e.what() << std::endl;
            return;
        }
    }
    else
    {
        boost::lock_guard<boost::mutex> lock(mtxCerr);
        std::cerr << "File " << filePath.string() << " is empty\n";
        return;
    }
    
    sum += number;

    boost::this_thread::sleep_for(boost::chrono::seconds(1));
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
    boost::atomic<int> totalSum(0);
    
    boost::thread_group threads;
    

    for (const auto& entry : fs::directory_iterator(targetPath))
    {
        if (fs::is_regular_file(entry)) 
        {
            threads.create_thread(boost::bind(&processFile, entry, std::ref(totalSum)));
        }
    }

    threads.join_all();

    std::cout << "The sum is " << totalSum << std::endl;
    
    std::cin.get();
    return 0;
}