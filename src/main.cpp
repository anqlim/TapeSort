#include <iostream>
#include <string>
#include "config/config.h"
#include "tape/tape_factory.h"
#include "sorter/tape_sorter.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: TapeSort <input_file> <output_file>\n";
        return 1;
    }

    const std::string inputPath = argv[1];
    const std::string outputPath = argv[2];

    try {
        Configuration::Config config("config.json");
        Tape::TapeFactory factory(config);
        auto input  = factory.openTape(inputPath);
        auto output = factory.createTape(outputPath, input->size());
        Sorting::TapeSorter sorter(config, factory);
        sorter.sort(*input, *output);
        std::cout << "Done. Sorted " << input->size() << " elements.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
