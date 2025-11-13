//g++ cli.cpp -o cli.exe -IC:\curl-8.9.0_1-win64-mingw\include -LC:\curl-8.9.0_1-win64-mingw\lib -lcurl -IC:\libraries\nlohmann
#include <iostream>
#include <string>
#include <iomanip>
#include <curl/curl.h>
#include <C:\libraries\nlohmann\json.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

using json = nlohmann::json;

std::string getUserName() {
    std::string username;

#ifdef _WIN32
    char buffer[256];
    DWORD buffer_size = sizeof(buffer);
    if (GetUserName(buffer, &buffer_size)) {
        username = buffer;
    }
#else
    char* login = getenv("LOGNAME");
    if (login) {
        username = login;
    } else {
        login = getenv("USER");
        if (login) {
            username = login;
        } else {
            struct passwd* pwd = getpwuid(getuid());
            if (pwd) {
                username = pwd->pw_name;
            }
        }
    }
#endif

    return username;
}

std::string urlEncode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
        }
    }

    return escaped.str();
}
// Function prototypes
void printHelp();
void greet(const std::string& name);
void createFile(const std::string& filename);
void readFile(const std::string& filename);
void deleteFile(const std::string& filename);
void printAbout();
std::vector<std::string> split(const std::string& str, char delimiter);
std::string trim(const std::string& str);
void getResearchPaperLinks(const std::string& topic);
void downloadPaper(const std::string& url, const std::string& filename);
void install(bool silent);

// Function to perform HTTP POST request using libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::string getRequest(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CAINFO, "C:\\libraries\\cacert.pem");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "curl_easy_init() failed." << std::endl;
    }

    curl_global_cleanup();

    return response;
}

void install(bool silent) {
    if (silent) {
        // Perform silent installation steps
        std::cout << "Installing silently...\n";
    } else {
        // Perform regular installation steps
        std::cout << "Installing with user interaction...\n";
    }
}

std::string fetchResearchSummary(const std::string& topic) {
    std::string encodedTopic = urlEncode(topic);
    std::string apiUrl = "http://export.arxiv.org/api/query?search_query=all:" + encodedTopic + "&start=0&max_results=1";

    std::string response = getRequest(apiUrl);
    if (response.empty()) return "";

    // Basic parsing for <summary>...</summary>
    size_t start = response.find("<summary>");
    size_t end = response.find("</summary>");
    if (start != std::string::npos && end != std::string::npos && end > start) {
        std::string summary = response.substr(start + 9, end - (start + 9));
        // Replace newlines and multiple spaces
        summary.erase(std::remove(summary.begin(), summary.end(), '\n'), summary.end());
        return trim(summary);
    }
    return "";
}


void summarizeTopic(const std::string& topic) {
    std::cout << "\nFetching summary for: " << topic << "...\n";

    std::string encodedTopic = urlEncode(topic);
    std::string apiUrl = "http://export.arxiv.org/api/query?search_query=all:" + encodedTopic + "&start=0&max_results=3";

    std::string xmlData = getRequest(apiUrl);
    if (xmlData.empty()) {
        std::cout << "No data found or failed to fetch.\n";
        return;
    }

    // Simple parse for <title> and <summary>
    size_t pos = 0;
    int count = 0;
    while ((pos = xmlData.find("<entry>", pos)) != std::string::npos && count < 3) {
        size_t titleStart = xmlData.find("<title>", pos) + 7;
        size_t titleEnd = xmlData.find("</title>", titleStart);
        std::string title = xmlData.substr(titleStart, titleEnd - titleStart);

        size_t sumStart = xmlData.find("<summary>", titleEnd) + 9;
        size_t sumEnd = xmlData.find("</summary>", sumStart);
        std::string summary = xmlData.substr(sumStart, sumEnd - sumStart);

        std::cout << "\nTitle: " << trim(title) << "\n";
        std::cout << "Summary: " << trim(summary) << "\n";

        pos = sumEnd;
        count++;
    }
}



int main(int argc, char* argv[]) {
    bool silent = false;

    // Check for silent installation flag
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "/S" || arg == "/silent" || arg == "/quiet") {
            silent = true;
            break;
        }
    }

    // If silent flag is present, perform installation and exit
    if (silent) {
        install(silent);
        return 0;
    }
    std::string username = getUserName();
    std::string command;
    std::cout << "Welcome, " << username << "! This is Chase, your personal SSE- Scientific Search Engine right within your own terminal.\nType 'help' for a list of commands.\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        command = trim(command);

        if (command.empty()) {
            continue;
        }

        if (command == "help") {
            printHelp();

        } else if (command.substr(0, 8) == "research") {
            if (command.length() > 9) {
              std::string topic = command.substr(9);
              bool summaryFlag = false;

              // Check for --summary flag
              size_t pos = topic.find("--summary");
              if (pos != std::string::npos) {
                  summaryFlag = true;
                  topic = trim(topic.substr(0, pos)); // remove flag
              }

              getResearchPaperLinks(topic);

              if (summaryFlag) {
                  std::cout << "\nFetching summary for: " << topic << "...\n";
                  std::string summaryText = fetchResearchSummary(topic); // new function
                  if (summaryText.empty()) {
                      std::cout << "No data found or failed to fetch.\n";
                  } else {
                      std::cout << summaryText << "\n";
                  }
              }
        } else {
                std::cout << "Usage: research <topic> [--summary]\n";
              } 
        } else if (command.substr(0, 6) == "create") {
            createFile(command.substr(7));
        } else if (command.substr(0, 4) == "read") {
            readFile(command.substr(5));
        } else if (command.substr(0, 6) == "delete") {
            deleteFile(command.substr(7));
        } else if (command == "about") {
            printAbout();
        } else if (command.substr(0, 8) == "download") {
            std::vector<std::string> parts = split(command.substr(9), ' ');
            if (parts.size() == 2) {
                downloadPaper(parts[0], parts[1]);
            } else {
                std::cout << "Usage: download <url> <filename>\n";
            }
        } else if (command == "exit") {
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Unknown command. Type 'help' for a list of commands.\n";
        }
    }

    return 0;
}

void printHelp() {
    std::cout << "Available commands:\n";
    std::cout << "help                       - Show this help message\n";
    std::cout << "research <topic>           - Get links to research papers on the specified topic\n";
    std::cout << "research <topic> --summary           - Get links *and* AI-generated summaries of research papers\n";
    std::cout << "download <url> <filename>  - Download a file from the specified URL\n";
    std::cout << "create <filename>          - Create a file with the specified name\n";
    std::cout << "read <filename>            - Read the contents of a file\n";
    std::cout << "delete <filename>          - Delete the specified file\n";
    std::cout << "about                      - Show information about the application\n";
    std::cout << "exit                       - Exit the application\n";
}

void downloadPaper(const std::string& url, const std::string& filename) {
    CURL* curl;
    CURLcode res;
    FILE* fp = fopen(filename.c_str(), "wb");

    if(fp == nullptr) {
        std::cerr << "Could not open file for writing: " << filename << std::endl;
        return;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        
        curl_easy_setopt(curl, CURLOPT_CAINFO, "C:\\libraries\\cacert.pem");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "curl_easy_init() failed." << std::endl;
    }

    fclose(fp);
    curl_global_cleanup();
}

void createFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file) {
        std::cout << "File " << filename << " created successfully.\n";
    } else {
        std::cout << "Failed to create file " << filename << ".\n";
    }
}

void readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "File " << filename << " not found.\n";
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << "Contents of " << filename << ":\n" << buffer.str() << "\n";
}

void deleteFile(const std::string& filename) {
    if (std::remove(filename.c_str()) == 0) {
        std::cout << "File " << filename << " deleted successfully.\n";
    } else {
        std::cout << "Failed to delete file " << filename << ".\n";
    }
}

void printAbout() {
    std::cout << "About SSE CLI Application:\n";
    std::cout << "Version: 1.0\n";
    std::cout << "Author: Daksh Sharma\n";
    std::cout << "Description: An intuitive command-line application that provides you relevant research paper analysis from different sites right from your terminal.\n";
    std::cout << "For more information, visit: http://frypan.vercel.app\n";
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

void getResearchPaperLinks(const std::string& topic) {
    std::string encodedTopic = urlEncode(topic);
    std::vector<std::string> links = {
        "https://arxiv.org/search/?query=" + encodedTopic + "&searchtype=all&source=header",
        "https://scholar.google.com/scholar?q=" + encodedTopic,
        "https://www.jstor.org/action/doBasicSearch?Query=" + encodedTopic,
        "https://ieeexplore.ieee.org/search/searchresult.jsp?newsearch=true&queryText=" + encodedTopic
    };

    std::cout << "Links to research papers on the topic '" << topic << "':\n";
    for (const auto& link : links) {
        std::cout << link << "\n";
    }
}

