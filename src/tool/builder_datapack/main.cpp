#include "../../engine/mlib/mlib.hpp"
#include "../../engine/file/filesystem.hpp"

int main(int argc, char* argv[])
{
    int32_t errCount = 0;
    Out.setFile("log.txt", false);
    Out.setOutput(true, true);
    if(argc < 3)
    {
        Out = "Usage:\n";
        Out = "pack_builder.exe data_pack_name file_001 file_002 [...] file_XXX\n";
    }
    else if(argc >= 3)
    {
        Out = "Building of " + std::string(argv[1]) + "\n";
        std::vector<std::string> file_list;
        for(int i = 2; i < argc; ++i)
            file_list.push_back(std::string(argv[i]));
        errCount = FileSystem::buildPack(std::string(argv[1]), file_list, true);
        Out = "Finished. " + mlib::int2str(errCount) + " error(s) encountered\n";
    }
    return errCount;
}
