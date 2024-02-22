#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

struct File {
    int id;
    std::string name;
    std::vector<std::string> categories;
    int numCategories;
    int parent;
    int size;
};

// Helper function for Task 3
int calcFileSize(int childId, std::unordered_map<int, int> &idSizes, std::unordered_map<int, 
            std::vector<int>> &parentChilds, std::unordered_map<int, File> &idFiles);

/**
 * Task 1
 */
std::vector<std::string> leafFiles(std::vector<File> files) {
    auto idFiles = std::unordered_map<int, File>();
    auto filesSet = std::unordered_set<int>();

    for (auto &file : files) {
        idFiles.emplace(file.id, file);
        filesSet.insert(file.id);
    }

    for (auto &file : files) {
        if (idFiles.find(file.parent) != idFiles.end()) {
            filesSet.erase(file.parent);
        }
    }

    auto leafFiles = std::vector<std::string>();

    for (auto &fileId : filesSet) {
        leafFiles.push_back(idFiles.find(fileId)->second.name);
    }

    return leafFiles;
}

/**
 * Task 2
 */
std::vector<std::string> kLargestCategories(std::vector<File> files, int k) {


    auto categoryCounts = std::unordered_map<std::string,int>();

    for (auto &file : files) {
        for (auto &category : file.categories) {
            categoryCounts[category] += 1;
        }
    }
    
    auto categories = std::vector<std::string>();

    for (int i = 0; i < k && !categoryCounts.empty(); ++i) {
        auto max = std::max_element(categoryCounts.begin(), categoryCounts.end(), 
        [] (std::pair<std::string, int> a, std::pair<std::string, int> b) {
            if (a.second == b.second) return a.first.compare(b.first) > 0;
            return a.second < b.second;
        });

        categories.push_back(max->first);
        categoryCounts.erase(max);
    }

    return categories;
}

/**
 * Task 3
 */
int largestFileSize(std::vector<File> files) {

    auto idFiles = std::unordered_map<int, File>();
    auto parentChilds = std::unordered_map<int, std::vector<int>>();
    for (auto &file : files) {
        idFiles.emplace(file.id, file);
        parentChilds[file.parent].push_back(file.id);
    }

    auto idSizes = std::unordered_map<int, int>();
    for (auto &file : files) {
        if (parentChilds.find(file.id) == parentChilds.end()) {
            // curr file is a leaf
            idSizes[file.id] = file.size;
        } else {
            idSizes[file.id] = -1;
        }
    }


    for (auto &idSize : idSizes) {
        calcFileSize(idSize.first, idSizes, parentChilds, idFiles);
    }


    return std::max_element(idSizes.begin(), idSizes.end(), 
        [] (std::pair<int, int> a, std::pair<int, int> b) {
            return a.second < b.second;
        })->second;
}

// Function finds the size of the current file in sort of a dp manner. 
// If the child of a parent has already been calculated then we can just use that value, otherwise
// we calculate the child at the same time and save that value, so we don't have to repeat this calc
int calcFileSize(int fileId, std::unordered_map<int, int> &idSizes, std::unordered_map<int, 
            std::vector<int>> &parentChilds, std::unordered_map<int, File> &idFiles) 
{
    if (idSizes[fileId] != -1) return idSizes[fileId];

    idSizes[fileId] = idFiles[fileId].size;

    for (auto &childId : parentChilds[fileId]) {
        idSizes[fileId] += calcFileSize(childId, idSizes, parentChilds, idFiles);
    }

    return idSizes[fileId];
}

int main(void) {
    std::vector<File> testFiles{
        { .id = 1, .name = "Document.txt", .categories = {"Documents"}, .numCategories = 1, .parent = 3, .size = 1024 },
        { .id = 2, .name = "Image.jpg", .categories = {"Media", "Photos"}, .numCategories = 2, .parent = 34, .size = 2048 },
        { .id = 3, .name = "Folder", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 0 },
        { .id = 5, .name = "Spreadsheet.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 4096 },
        { .id = 8, .name = "Backup.zip", .categories = {"Backup"}, .numCategories = 1, .parent = 233, .size = 8192 },
        { .id = 13, .name = "Presentation.pptx", .categories = {"Documents", "Presentation"}, .numCategories = 2, .parent = 3, .size = 3072 },
        { .id = 21, .name = "Video.mp4", .categories = {"Media", "Videos"}, .numCategories = 2, .parent = 34, .size = 6144 },
        { .id = 34, .name = "Folder2", .categories = {"Folder"}, .numCategories = 1, .parent = 3, .size = 0 },
        { .id = 55, .name = "Code.py", .categories = {"Programming"}, .numCategories = 1, .parent = -1, .size = 1536 },
        { .id = 89, .name = "Audio.mp3", .categories = {"Media", "Audio"}, .numCategories = 2, .parent = 34, .size = 2560 },
        { .id = 144, .name = "Spreadsheet2.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 2048 },
        { .id = 233, .name = "Folder3", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 4096 },
    };

    std::vector<std::string> expectedLeafFiles{
        "Audio.mp3",
        "Backup.zip",
        "Code.py",
        "Document.txt",
        "Image.jpg",
        "Presentation.pptx",
        "Spreadsheet.xlsx",
        "Spreadsheet2.xlsx",
        "Video.mp4"
    };
    std::vector<std::string> returnedLeafFiles = leafFiles(testFiles);
    std::sort(returnedLeafFiles.begin(), returnedLeafFiles.end());
    assert(expectedLeafFiles == returnedLeafFiles);

    std::vector<std::string> expectedCategories{"Documents", "Folder", "Media"};
    std::vector<std::string> returnedCategories = kLargestCategories(testFiles, 3);
    assert(expectedCategories == returnedCategories);

    assert(largestFileSize(testFiles) == 20992);

}