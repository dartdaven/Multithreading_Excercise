#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <chrono>
#include <mutex>

namespace fs = std::filesystem;

std::mutex mtx;

void processFile(const fs::path& filePath, int& sum)
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

            std::lock_guard<std::mutex> lock(mtx);
            std::cout << filePath.filename().string() << ": " << number << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Wrong content in the file: " << filePath.string() << " with: " << e.what() << std::endl;
            return;
        }
    }
    else
    {
        std::cerr << "File " << filePath.string() << " is empty\n";
        return;
    }
    
    //modifing outer variable
    std::unique_lock<std::mutex> lock(mtx);
    
    sum += number;
    
    lock.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(1));
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
        std::cout << "Please enter the folder path\n";
        std::getline(std::cin, targetPath);
    }

    if (targetPath.empty() || !fs::exists(targetPath)) 
    { 
        targetPath = "Test";
        if (!fs::exists(targetPath))
        {
            std::cerr << "Unfortunatelly there was no \"Test\" folder found nearby\n";
            std::cin.get();
            return 0;
        }
        std::cout << "Default folder was chosen\n";
    }

    //multithreading part
    std::vector<std::thread> threads;
    int totalSum{ 0 };

    for (const auto& entry : fs::directory_iterator(targetPath))
    {
        if (fs::is_regular_file(entry)) 
        {
            threads.emplace_back(processFile, entry.path(), std::ref(totalSum));
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