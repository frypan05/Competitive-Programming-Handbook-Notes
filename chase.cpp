//g++ cli.cpp -o cli.exe -IC:\curl-8.9.0_1-win64-mingw\include -LC:\curl-8.9.0_1-win64-mingw\lib -lcurl -IC:\libraries\nlohmann

#include <iostream>
#include <string>
#include <iomanip>
#include <curl/curl.h>
#include <C:\libraries\nlohmann\json.hpp>
#include <regex>
#include <thread>
#include <chrono>

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
namespace fs = std::filesystem;

// Version and branding
const std::string VERSION = "2.0.0";
const std::string APP_NAME = "Chase SSE";
const std::string AUTHOR = "Daksh Sharma";

// Configuration structure
struct Config {
    std::string cacheDir;
    std::string downloadDir;
    int maxResults = 10;
    bool enableCache = true;
    std::string userAgent = "Chase-SSE/2.0";
};

Config appConfig;

// Initialize directories
void initializeDirectories() {
    std::string homeDir;
#ifdef _WIN32
    char* appData = std::getenv("APPDATA");
    if (appData) {
        homeDir = std::string(appData) + "\\Chase-SSE";
    } else {
        homeDir = ".\\chase-sse";
    }
#else
    char* home = std::getenv("HOME");
    if (home) {
        homeDir = std::string(home) + "/.chase-sse";
    } else {
        homeDir = "./chase-sse";
    }
#endif
    
    appConfig.cacheDir = homeDir + "/cache";
    appConfig.downloadDir = homeDir + "/downloads";
    
    try {
        fs::create_directories(appConfig.cacheDir);
        fs::create_directories(appConfig.downloadDir);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not create directories: " << e.what() << std::endl;
    }
}

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
void printVersion();
std::vector<std::string> split(const std::string& str, char delimiter);
std::string trim(const std::string& str);
void getResearchPaperLinks(const std::string& topic);
void downloadPaper(const std::string& url, const std::string& filename);
void install(bool silent);
void searchArxiv(const std::string& query, bool detailed = false);
void clearCache();
void showStats();
void configureApp();
bool isValidUrl(const std::string& url);

// Progress callback for downloads
int progressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal > 0) {
        double percentage = (double)dlnow / (double)dltotal * 100.0;
        std::cout << "\rProgress: [";
        int pos = (int)(percentage / 2);
        for (int i = 0; i < 50; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "%" << std::flush;
    }
    return 0;
}

// HTTP callbacks
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

// Improved HTTP request with better error handling and caching
std::string getRequest(const std::string& url, bool useCache = true) {
    // Check cache first
    if (useCache && appConfig.enableCache) {
        std::string cacheFile = appConfig.cacheDir + "/cache_" + std::to_string(std::hash<std::string>{}(url));
        if (fs::exists(cacheFile)) {
            auto cacheTime = fs::last_write_time(cacheFile);
            auto now = fs::file_time_type::clock::now();
            auto age = std::chrono::duration_cast<std::chrono::hours>(now - cacheTime);
            
            if (age.count() < 24) { // Cache valid for 24 hours
                std::ifstream cache(cacheFile);
                if (cache.is_open()) {
                    std::string cached_content((std::istreambuf_iterator<char>(cache)),
                                             std::istreambuf_iterator<char>());
                    std::cout << " [cached] ";
                    return cached_content;
                }
            }
        }
    }

    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // Set headers
        struct curl_slist *headers = nullptr;
        std::string userAgent = "User-Agent: " + appConfig.userAgent;
        headers = curl_slist_append(headers, userAgent.c_str());
        headers = curl_slist_append(headers, "Accept: application/xml,text/xml,*/*");
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // For now, disable SSL verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "\nHTTP request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "curl_easy_init() failed." << std::endl;
    }

    curl_global_cleanup();

    // Cache the response
    if (!response.empty() && useCache && appConfig.enableCache) {
        std::string cacheFile = appConfig.cacheDir + "/cache_" + std::to_string(std::hash<std::string>{}(url));
        std::ofstream cache(cacheFile);
        if (cache.is_open()) {
            cache << response;
        }
    }

    return response;
}

void install(bool silent) {
    if (silent) {
        std::cout << "Installing Chase SSE silently...\n";
        initializeDirectories();
        std::cout << "Installation completed successfully.\n";
    } else {
        std::cout << "Installing Chase SSE with user interaction...\n";
        std::cout << "Setting up directories...\n";
        initializeDirectories();
        std::cout << "Installation completed! Welcome to Chase SSE!\n";
        std::cout << "Type 'help' to get started.\n";
    }
}

// Enhanced ArXiv search with proper XML parsing
void searchArxiv(const std::string& query, bool detailed) {
    std::cout << "\nSearching ArXiv for: '" << query << "'";
    std::string encodedQuery = urlEncode(query);
    std::string apiUrl = "http://export.arxiv.org/api/query?search_query=all:" + 
                        encodedQuery + "&start=0&max_results=" + std::to_string(appConfig.maxResults);

    std::string xmlData = getRequest(apiUrl);
    if (xmlData.empty()) {
        std::cout << "\nNo data found or failed to fetch from ArXiv.\n";
        return;
    }

    // Check if we got valid XML response
    if (xmlData.find("<?xml") == std::string::npos && xmlData.find("<feed") == std::string::npos) {
        std::cout << "\nReceived invalid response from ArXiv API.\n";
        return;
    }

    // Parse entries
    std::vector<std::string> entries;
    size_t pos = 0;
    while ((pos = xmlData.find("<entry>", pos)) != std::string::npos) {
        size_t endPos = xmlData.find("</entry>", pos);
        if (endPos != std::string::npos) {
            entries.push_back(xmlData.substr(pos, endPos - pos + 8));
            pos = endPos + 8;
        } else {
            break;
        }
    }

    if (entries.empty()) {
        std::cout << "\nNo research papers found for the given topic.\n";
        return;
    }

    std::cout << "\n\nFound " << entries.size() << " research papers:\n";
    std::cout << std::string(80, '=') << "\n";

    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        
        // Extract title
        std::regex titleRegex("<title>(.*?)</title>");
        std::smatch titleMatch;
        std::string title = "Unknown Title";
        if (std::regex_search(entry, titleMatch, titleRegex)) {
            title = titleMatch[1].str();
            title = std::regex_replace(title, std::regex("\\n\\s+"), " ");
            title = trim(title);
        }

        // Extract authors
        std::regex authorRegex("<name>(.*?)</name>");
        std::sregex_iterator authorIt(entry.begin(), entry.end(), authorRegex);
        std::sregex_iterator authorEnd;
        std::vector<std::string> authors;
        for (; authorIt != authorEnd; ++authorIt) {
            authors.push_back(trim((*authorIt)[1].str()));
        }

        // Extract ArXiv ID and PDF link
        std::regex idRegex("arxiv.org/abs/(\\d+\\.\\d+)");
        std::smatch idMatch;
        std::string arxivId;
        std::string pdfUrl;
        if (std::regex_search(entry, idMatch, idRegex)) {
            arxivId = idMatch[1].str();
            pdfUrl = "https://arxiv.org/pdf/" + arxivId + ".pdf";
        }

        // Extract publication date
        std::regex dateRegex("<published>(\\d{4}-\\d{2}-\\d{2})");
        std::smatch dateMatch;
        std::string pubDate = "Unknown";
        if (std::regex_search(entry, dateMatch, dateRegex)) {
            pubDate = dateMatch[1].str();
        }

        std::cout << "\n[" << (i + 1) << "] " << title << "\n";
        if (!authors.empty()) {
            std::cout << "Authors: ";
            for (size_t j = 0; j < authors.size() && j < 3; ++j) {
                std::cout << authors[j];
                if (j < std::min(authors.size(), (size_t)3) - 1) std::cout << ", ";
            }
            if (authors.size() > 3) std::cout << " et al.";
            std::cout << "\n";
        }
        std::cout << "Published: " << pubDate << "\n";
        if (!arxivId.empty()) {
            std::cout << "ArXiv ID: " << arxivId << "\n";
            std::cout << "PDF: " << pdfUrl << "\n";
        }

        if (detailed) {
            // Extract summary
            std::regex summaryRegex("<summary>(.*?)</summary>", std::regex_constants::extended);
            std::smatch summaryMatch;
            if (std::regex_search(entry, summaryMatch, summaryRegex)) {
                std::string summary = summaryMatch[1].str();
                summary = std::regex_replace(summary, std::regex("\\n\\s+"), " ");
                summary = trim(summary);
                if (summary.length() > 300) {
                    summary = summary.substr(0, 300) + "...";
                }
                std::cout << "Summary: " << summary << "\n";
            }
        }
        
        std::cout << std::string(40, '-') << "\n";
    }

    std::cout << "\nTip: Use 'download <pdf_url> <filename>' to download papers\n";
    std::cout << "Tip: Use 'research <topic> --detailed' for paper summaries\n";
}

// Enhanced research function
void getResearchPaperLinks(const std::string& topic, bool detailed = false) {
    std::cout << " Searching multiple databases for: '" << topic << "'\n\n";
    
    // First show ArXiv results with detailed info
    searchArxiv(topic, detailed);
    
    // Then show other database links
    std::cout << "\n Additional Research Databases:\n";
    std::cout << std::string(50, '=') << "\n";
    
    std::string encodedTopic = urlEncode(topic);
    std::vector<std::pair<std::string, std::string>> links = {
        {"Google Scholar", "https://scholar.google.com/scholar?q=" + encodedTopic},
        {"JSTOR", "https://www.jstor.org/action/doBasicSearch?Query=" + encodedTopic},
        {"IEEE Xplore", "https://ieeexplore.ieee.org/search/searchresult.jsp?newsearch=true&queryText=" + encodedTopic},
        {"PubMed", "https://pubmed.ncbi.nlm.nih.gov/?term=" + encodedTopic},
        {"Semantic Scholar", "https://www.semanticscholar.org/search?q=" + encodedTopic},
        {"ResearchGate", "https://www.researchgate.net/search?q=" + encodedTopic}
    };

    for (const auto& [name, url] : links) {
        std::cout << "• " << std::left << std::setw(20) << name << ": " << url << "\n";
    }
}

bool isValidUrl(const std::string& url) {
    std::regex urlRegex(
        R"(^https?://[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}(/.*)?$)"
    );
    return std::regex_match(url, urlRegex);
}

void downloadPaper(const std::string& url, const std::string& filename) {
    if (!isValidUrl(url)) {
        std::cout << " Invalid URL format.\n";
        return;
    }

    std::string fullPath = appConfig.downloadDir + "/" + filename;
    
    std::cout << " Downloading: " << url << "\n";
    std::cout << " Saving to: " << fullPath << "\n";

    CURL* curl;
    CURLcode res;
    FILE* fp = fopen(fullPath.c_str(), "wb");

    if(fp == nullptr) {
        std::cerr << " Could not open file for writing: " << fullPath << std::endl;
        return;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L); // 5 minutes timeout
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallback);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);

        std::cout << "\n"; // New line after progress bar

        if(res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code == 200) {
                std::cout << " Download completed successfully!\n";
                std::cout << " File saved at: " << fullPath << "\n";
            } else {
                std::cout << "⚠️  Download completed with HTTP code: " << response_code << "\n";
            }
        } else {
            std::cout << " Download failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << " curl_easy_init() failed." << std::endl;
    }

    fclose(fp);
    curl_global_cleanup();
}

void clearCache() {
    try {
        int removed = 0;
        for (const auto& entry : fs::directory_iterator(appConfig.cacheDir)) {
            if (entry.is_regular_file()) {
                fs::remove(entry.path());
                removed++;
            }
        }
        std::cout << "  Cleared " << removed << " cache files.\n";
    } catch (const std::exception& e) {
        std::cout << " Error clearing cache: " << e.what() << "\n";
    }
}

void showStats() {
    std::cout << " Chase SSE Statistics:\n";
    std::cout << std::string(30, '=') << "\n";
    std::cout << "Version: " << VERSION << "\n";
    std::cout << "Cache Directory: " << appConfig.cacheDir << "\n";
    std::cout << "Download Directory: " << appConfig.downloadDir << "\n";
    std::cout << "Max Results: " << appConfig.maxResults << "\n";
    std::cout << "Cache Enabled: " << (appConfig.enableCache ? "Yes" : "No") << "\n";
    
    // Count cache files
    try {
        int cacheFiles = 0;
        uintmax_t cacheSize = 0;
        for (const auto& entry : fs::directory_iterator(appConfig.cacheDir)) {
            if (entry.is_regular_file()) {
                cacheFiles++;
                cacheSize += entry.file_size();
            }
        }
        std::cout << "Cache Files: " << cacheFiles << " (" << (cacheSize / 1024) << " KB)\n";
    } catch (...) {
        std::cout << "Cache Files: Unable to read\n";
    }
    
    // Count downloads
    try {
        int downloadFiles = 0;
        for (const auto& entry : fs::directory_iterator(appConfig.downloadDir)) {
            if (entry.is_regular_file()) {
                downloadFiles++;
            }
        }
        std::cout << "Downloaded Files: " << downloadFiles << "\n";
    } catch (...) {
        std::cout << "Downloaded Files: Unable to read\n";
    }
}

void configureApp() {
    std::cout << "⚙️  Chase SSE Configuration:\n";
    std::cout << std::string(30, '=') << "\n";
    
    std::cout << "Current max results per search: " << appConfig.maxResults << "\n";
    std::cout << "Enter new value (1-50, press Enter to keep current): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            int newMax = std::stoi(input);
            if (newMax >= 1 && newMax <= 50) {
                appConfig.maxResults = newMax;
                std::cout << " Max results updated to: " << newMax << "\n";
            } else {
                std::cout << " Invalid value. Must be between 1-50.\n";
            }
        } catch (...) {
            std::cout << " Invalid input.\n";
        }
    }
    
    std::cout << "\nCache currently " << (appConfig.enableCache ? "enabled" : "disabled") << "\n";
    std::cout << "Toggle cache? (y/n, press Enter to keep current): ";
    std::getline(std::cin, input);
    if (input == "y" || input == "Y") {
        appConfig.enableCache = !appConfig.enableCache;
        std::cout << " Cache " << (appConfig.enableCache ? "enabled" : "disabled") << "\n";
    }
}

int main(int argc, char* argv[]) {
    bool silent = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "/S" || arg == "/silent" || arg == "/quiet") {
            silent = true;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
    }

    // If silent flag is present, perform installation and exit
    if (silent) {
        install(silent);
        return 0;
    }

    // Initialize the application
    initializeDirectories();

    std::string username = getUserName();
    std::string command;
    
    std::cout << " Welcome, " << username << "!\n";
    std::cout << "This is " << APP_NAME << " v" << VERSION << " - Your Scientific Search Engine\n";
    std::cout << "Type 'help' for available commands, 'about' for more info.\n\n";

    while (true) {
        std::cout << "chase> ";
        std::getline(std::cin, command);
        command = trim(command);

        if (command.empty()) {
            continue;
        }

        // Parse commands
        std::vector<std::string> args = split(command, ' ');
        std::string cmd = args[0];

        if (cmd == "help" || cmd == "h") {
            printHelp();
        
        } else if (cmd == "research" || cmd == "search" || cmd == "r") {
            if (args.size() < 2) {
                std::cout << "Usage: research <topic> [--detailed]\n";
                continue;
            }
            
            std::string topic;
            bool detailed = false;
            
            // Rebuild topic from args, checking for flags
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] == "--detailed" || args[i] == "--summary" || args[i] == "-d") {
                    detailed = true;
                } else {
                    if (!topic.empty()) topic += " ";
                    topic += args[i];
                }
            }
            
            if (!topic.empty()) {
                getResearchPaperLinks(topic, detailed);
            } else {
                std::cout << "Please specify a research topic.\n";
            }
        
        } else if (cmd == "download" || cmd == "dl") {
            if (args.size() < 3) {
                std::cout << "Usage: download <url> <filename>\n";
            } else {
                downloadPaper(args[1], args[2]);
            }
        
        } else if (cmd == "create") {
            if (args.size() < 2) {
                std::cout << "Usage: create <filename>\n";
            } else {
                createFile(args[1]);
            }
        
        } else if (cmd == "read") {
            if (args.size() < 2) {
                std::cout << "Usage: read <filename>\n";
            } else {
                readFile(args[1]);
            }
        
        } else if (cmd == "delete" || cmd == "del") {
            if (args.size() < 2) {
                std::cout << "Usage: delete <filename>\n";
            } else {
                deleteFile(args[1]);
            }
        
        } else if (cmd == "clear-cache" || cmd == "clearcache") {
            clearCache();
        
        } else if (cmd == "stats" || cmd == "status") {
            showStats();
        
        } else if (cmd == "config" || cmd == "configure") {
            configureApp();
        
        } else if (cmd == "about") {
            printAbout();
        
        } else if (cmd == "version" || cmd == "--version") {
            printVersion();
        
        } else if (cmd == "exit" || cmd == "quit" || cmd == "q") {
            std::cout << " Thanks for using Chase SSE! Goodbye!\n";
            break;
        
        } else {
            std::cout << " Unknown command: '" << cmd << "'. Type 'help' for available commands.\n";
        }
    }

    return 0;
}

void printVersion() {
    std::cout << APP_NAME << " v" << VERSION << "\n";
    std::cout << "Built for scientific research and paper discovery.\n";
}

void printHelp() {
    std::cout << "\n " << APP_NAME << " v" << VERSION << " - Available Commands:\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "  SEARCH COMMANDS:\n";
    std::cout << "  research <topic>          - Search for research papers\n";
    std::cout << "  research <topic> -d       - Search with detailed summaries\n";
    std::cout << "  r <topic>                 - Short alias for research\n\n";
    
    std::cout << "  DOWNLOAD COMMANDS:\n";
    std::cout << "  download <url> <filename> - Download a paper/file\n";
    std::cout << "  dl <url> <filename>       - Short alias for download\n\n";
    
    std::cout << "  FILE COMMANDS:\n";
    std::cout << "  create <filename>         - Create a new file\n";
    std::cout << "  read <filename>           - Read file contents\n";
    std::cout << "  delete <filename>         - Delete a file\n\n";
    
    std::cout << "⚙️ SYSTEM COMMANDS:\n";
    std::cout << "  config                    - Configure application settings\n";
    std::cout << "  stats                     - Show application statistics\n";
    std::cout << "  clear-cache               - Clear cached search results\n";
    std::cout << "  about                     - Show application information\n";
    std::cout << "  version                   - Show version information\n";
    std::cout << "  help                      - Show this help message\n";
    std::cout << "  exit                      - Exit the application\n\n";
    
    std::cout << "  TIPS:\n";
    std::cout << "  - Use quotes for multi-word topics: research \"machine learning\"\n";
    std::cout << "  - Downloads are saved to: " << appConfig.downloadDir << "\n";
    std::cout << "  - Cache improves search speed and reduces API calls\n";
    std::cout << "  - Use --detailed flag for paper summaries and more info\n";
}

void createFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file) {
        std::cout << " File '" << filename << "' created successfully.\n";
    } else {
        std::cout << " Failed to create file '" << filename << "'.\n";
    }
}

void readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << " File '" << filename << "' not found.\n";
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << " Contents of '" << filename << "':\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << buffer.str() << "\n";
    std::cout << std::string(40, '-') << "\n";
}

void deleteFile(const std::string& filename) {
    if (std::remove(filename.c_str()) == 0) {
        std::cout << "  File '" << filename << "' deleted successfully.\n";
    } else {
        std::cout << "  Failed to delete file '" << filename << "'.\n";
    }
}

void printAbout() {
    std::cout << "\n  About " << APP_NAME << ":\n";
    std::cout << std::string(40, '=') << "\n";
    std::cout << "Version: " << VERSION << "\n";
    std::cout << "Author: " << AUTHOR << "\n";
    std::cout << "Description: An advanced command-line scientific search engine\n";
    std::cout << "             that provides comprehensive research paper discovery\n";
    std::cout << "             and analysis from multiple academic databases.\n\n";
    
    std::cout << "  KEY FEATURES:\n";
    std::cout << "  • Multi-database search (ArXiv, Google Scholar, IEEE, etc.)\n";
    std::cout << "  • Real-time paper summaries and metadata extraction\n";
    std::cout << "  • Intelligent caching for faster searches\n";
    std::cout << "  • Direct PDF download capabilities\n";
    std::cout << "  • Cross-platform compatibility (Windows, Linux, macOS)\n";
    std::cout << "  • Configurable search parameters\n";
    std::cout << "  • Progress tracking for downloads\n\n";
    
    std::cout << "  LINKS:\n";
    std::cout << "  Website: https://dakshsharma.me\n";
    std::cout << "  GitHub: https://github.com/frypan05/chase-sse\n";
    std::cout << "  Issues: https://github.com/frypan05/chase-sse/issues\n\n";
    
    std::cout << "  CONTACT:\n";
    std::cout << "  For support, feature requests, or bug reports,\n";
    std::cout << "  please visit our GitHub repository or website.\n\n";
    
    std::cout << "   If you find Chase SSE helpful, please consider starring\n";
    std::cout << "   our repository and sharing it with fellow researchers!\n";
}

// Utility functions
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
