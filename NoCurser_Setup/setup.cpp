#include <iostream>     // For input and output streams
#include <fstream>      // For file input/output operations
#include <string>       // For std::string manipulation
#include <vector>       // For std::vector container
#include <filesystem>   // For filesystem operations (C++17)
#include <regex>        // For regular expressions

// Namespace alias for convenience
namespace fs = std::filesystem;

/**
 * @brief Replace all occurrences of a regex pattern within a file with a replacement string.
 * 
 * @param filepath The path to the file to modify.
 * @param pattern The regex pattern to search for.
 * @param replacement The string to replace matches with.
 * @return true If the file was successfully modified or no changes were needed.
 * @return false If an error occurred reading or writing the file.
 */
bool replace_in_file(const std::string& filepath, const std::regex& pattern, const std::string& replacement) {
    std::ifstream file_in(filepath);
    if (!file_in) {
        std::cerr << "[ERROR] Unable to open file for reading: " << filepath << "\n";
        return false;
    }

    // Read entire file content into a string
    std::string content((std::istreambuf_iterator<char>(file_in)), std::istreambuf_iterator<char>());
    file_in.close();

    // Perform regex replacement on the content
    std::string modified_content = std::regex_replace(content, pattern, replacement);

    // If no change, inform and return success
    if (modified_content == content) {
        std::cout << "[INFO] No changes needed in " << filepath << "\n";
        return true;
    }

    // Write modified content back to the file
    std::ofstream file_out(filepath);
    if (!file_out) {
        std::cerr << "[ERROR] Unable to open file for writing: " << filepath << "\n";
        return false;
    }
    file_out << modified_content;
    file_out.close();

    std::cout << "[SUCCESS] Modified file: " << filepath << "\n";
    return true;
}

/**
 * @brief Append a snippet of text to a file if it does not already contain it.
 * 
 * @param filepath The path to the file to append to.
 * @param snippet The text snippet to append.
 * @return true If the snippet was appended or already present.
 * @return false If an error occurred.
 */
bool append_if_missing(const std::string& filepath, const std::string& snippet) {
    std::ifstream file_in(filepath);
    if (!file_in) {
        std::cerr << "[ERROR] Unable to open file for reading: " << filepath << "\n";
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file_in)), std::istreambuf_iterator<char>());
    file_in.close();

    // Check if snippet already exists
    if (content.find(snippet) != std::string::npos) {
        std::cout << "[INFO] Snippet already present in " << filepath << "\n";
        return true;
    }

    // Append the snippet to the file
    std::ofstream file_out(filepath, std::ios::app);
    if (!file_out) {
        std::cerr << "[ERROR] Unable to open file for appending: " << filepath << "\n";
        return false;
    }
    file_out << "\n" << snippet << "\n";
    file_out.close();

    std::cout << "[SUCCESS] Appended snippet to " << filepath << "\n";
    return true;
}

/**
 * @brief Entry point of the program. Applies modifications to Vim source files
 *        to create a minimalistic 'Vim-NoCursor' fork.
 * 
 * @return int Exit code: 0 on success, non-zero on failure.
 */
int main() {
    std::cout << "[START] Initiating Vim-NoCursor source code adjustments...\n";

    // Step 1: Rename the output binary in the Makefile to 'nocursor'
    if (!replace_in_file(
            "src/Makefile",
            std::regex(R"(^(TARGET\s*=\s*).*)"),
            "$1nocursor")) {
        return 1;
    }

    // Step 2: Update version macros in 'src/version.c' for branding
    if (!replace_in_file(
            "src/version.c",
            std::regex(R"(#define\s+VIM_VERSION_MEDIUM\s+\".*\")"),
            "#define VIM_VERSION_MEDIUM  \"NoCursor\"")) {
        return 1;
    }

    if (!replace_in_file(
            "src/version.c",
            std::regex(R"(#define\s+VIM_VERSION_LONG\s+\".*\")"),
            "#define VIM_VERSION_LONG    \"Vim-NoCursor v0.1 Minimal\"")) {
        return 1;
    }

    // Step 3: Replace the splash screen function 'do_intro()' in 'src/version.c'
    const std::string splash_function =
        "void do_intro() {\n"
        "    msg_puts(\"\\nVim-NoCursor v0.1 Minimal\\n\");\n"
        "    msg_puts(\"https://github.com/braga-waysward/Vim-NoCurser\\n\\n\");\n"
        "}";

    if (!replace_in_file(
            "src/version.c",
            std::regex(R"(void\s+do_intro\s*\(\)\s*{[^}]*})"),
            splash_function)) {
        return 1;
    }

    // Step 4: Add the 'hide_cursor()' function in 'src/term.c' if not present
    const std::string hide_cursor_code =
        "// Hide terminal cursor\n"
        "void hide_cursor() {\n"
        "    out_str(\"\\033[?25l\");\n"
        "}";

    if (!append_if_missing("src/term.c", hide_cursor_code)) {
        return 1;
    }

    // Step 5: Insert call to 'hide_cursor()' in 'src/main.c' after 'mch_exitsetup();'
    std::ifstream main_file_in("src/main.c");
    if (!main_file_in) {
        std::cerr << "[ERROR] Unable to open 'src/main.c' for reading.\n";
        return 1;
    }
    std::string main_content((std::istreambuf_iterator<char>(main_file_in)), std::istreambuf_iterator<char>());
    main_file_in.close();

    if (main_content.find("hide_cursor();") == std::string::npos) {
        std::regex mch_exitsetup_regex(R"(mch_exitsetup\s*\(\s*\)\s*;)");
        std::string modified_main = std::regex_replace(main_content, mch_exitsetup_regex, "mch_exitsetup();\n    hide_cursor();");

        std::ofstream main_file_out("src/main.c");
        if (!main_file_out) {
            std::cerr << "[ERROR] Unable to open 'src/main.c' for writing.\n";
            return 1;
        }
        main_file_out << modified_main;
        main_file_out.close();

        std::cout << "[SUCCESS] Inserted 'hide_cursor()' call into 'src/main.c'.\n";
    } else {
        std::cout << "[INFO] 'hide_cursor()' call already present in 'src/main.c'.\n";
    }

    // Step 6: Adjust default option values in 'src/option.c' to enforce minimalism
    const std::vector<std::pair<std::regex, std::string>> options_modifications = {
        {std::regex(R"(p_number\s*=\s*\d+;)"), "p_number = 0;"},
        {std::regex(R"(p_ruler\s*=\s*\d+;)"), "p_ruler = 0;"},
        {std::regex(R"(p_showcmd\s*=\s*\d+;)"), "p_showcmd = 0;"},
        {std::regex(R"(p_ls\s*=\s*\d+;)"), "p_ls = 0;"},
        {std::regex(R"(p_laststatus\s*=\s*\d+;)"), "p_laststatus = 0;"},
        {std::regex(R"(p_showmode\s*=\s*\d+;)"), "p_showmode = 0;"},
        {std::regex(R"(p_cmdheight\s*=\s*\d+;)"), "p_cmdheight = 1;"}
    };

    for (const auto& [pattern, replacement] : options_modifications) {
        if (!replace_in_file("src/option.c", pattern, replacement)) {
            return 1;
        }
    }

    // Step 7: Comment out calls to 'showmode()' in 'src/normal.c' to disable mode display
    std::ifstream normal_file_in("src/normal.c");
    if (!normal_file_in) {
        std::cerr << "[ERROR] Unable to open 'src/normal.c' for reading.\n";
        return 1;
    }
    std::string normal_content((std::istreambuf_iterator<char>(normal_file_in)), std::istreambuf_iterator<char>());
    normal_file_in.close();

    if (normal_content.find("//showmode();") == std::string::npos) {
        std::regex showmode_regex(R"(^(\s*)showmode\s*\(\s*\)\s*;)");
        std::string modified_normal = std::regex_replace(normal_content, showmode_regex, "$1//showmode();");

        std::ofstream normal_file_out("src/normal.c");
        if (!normal_file_out) {
            std::cerr << "[ERROR] Unable to open 'src/normal.c' for writing.\n";
            return 1;
        }
        normal_file_out << modified_normal;
        normal_file_out.close();

        std::cout << "[SUCCESS] Commented out 'showmode()' calls in 'src/normal.c'.\n";
    } else {
        std::cout << "[INFO] 'showmode()' calls already commented in 'src/normal.c'.\n";
    }

    std::cout << "\n[COMPLETE] Vim-NoCursor adjustments finished successfully.\n";
    std::cout << "Please run 'make' inside the repository root to build your minimal, cursorless Vim.\n";

    return 0;
}
