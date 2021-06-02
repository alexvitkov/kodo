#include <InputFile.h>
#include <Error.h>

std::vector<InputFile*> input_files;



static char* readfile(const char* file) {
    FILE* f = fopen(file, "rb");
    if (!f) {
        return nullptr;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return nullptr;
    }

    long length = ftell(f);
    rewind(f);
    char* buf = (char*)malloc(length + 1);
    buf[length] = 0; // the buffer we pass to lex must be 0-terminated

    int nread = fread(buf, 1, length, f);
    if (nread < length) {
        fclose(f);
        free(buf);
        return nullptr;
    }

    fclose(f);
    return buf;
}


InputFile* load_file(std::string path) {
    char* f = readfile(path.c_str());
    if (!f) {
        add_error(new FailedToReadInputFileError(path));
        return nullptr;
    }

    InputFile* input = new InputFile();
    input->buffer = f;

    input_files.push_back(input);
    return input;
}
