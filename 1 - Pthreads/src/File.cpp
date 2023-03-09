#include "File.hpp"

File::File(string file_path, int size)
{
    this->file_path = file_path;
    this->size = size;
}

File::File() : File::File("", 0) {}

File::File(string file_path) : File::File(file_path, compute_file_size(file_path)) {}

long File::compute_file_size()
{
    ifstream file(this->file_path, ios::binary);
    file.seekg(0, ios::end);
    return file.tellg();
}

long File::compute_file_size(string file_path)
{
    ifstream file(file_path, ios::binary);
    file.seekg(0, ios::end);
    return file.tellg();
}

bool File::size_sort(File *f1, File *f2)
{
    return (f1->size < f2->size);
}