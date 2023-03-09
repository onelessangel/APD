#ifndef _FILE_HPP_
#define _FILE_HPP_

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class File
{
  public:
    string file_path;
    long size;
    
    File();
    File(string file_path, int size);
    File(string file_path);

    long compute_file_size();
    long compute_file_size(string file_path);
    static bool size_sort(File *f1, File *f2);
};

#endif