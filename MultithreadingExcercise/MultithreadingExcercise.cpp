#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <chrono>

namespace fs = std::filesystem;

void processFile(const std::string& fileName, int& sum)
{
    std::ifstream file(fileName);
    int number;
    
    //TO DO an additional check
    file >> number;

    //TO DO only filename without path
    std::cout << fileName << ": " << number << std::endl;
    sum += number;

    std::this_thread::sleep_for(std::chrono::seconds(1));
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

    std::vector<std::thread> threads;
    int totalSum{ 0 };

    for (const auto& entry : fs::directory_iterator(targetPath))
    {
        if (fs::is_regular_file(entry)) 
        {
            threads.emplace_back(processFile, entry.path().string(), std::ref(totalSum));
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