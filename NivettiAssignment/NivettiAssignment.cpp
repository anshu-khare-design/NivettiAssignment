// NivettiAssignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime> 
#include <iomanip>
#include <windows.h>
#include <Lmcons.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include<vector>
using namespace std;

bool writing_syslog = false;
bool writing_auth = false;

mutex syslog;

condition_variable cv1;

mutex auth;
condition_variable cv2;

void create_file(string file_type, string output_file_name)
{
    fstream in_file;

    in_file.open(file_type, std::fstream::in | std::fstream::out | std::fstream::app);

    if (!in_file)
    {
        cout << "Input file does not exist!" << endl;
        return;
    }




    fstream op_file;

    op_file.open(output_file_name, std::fstream::in | std::fstream::out | std::fstream::app);

    if (!op_file)
    {
        cout << "Error in creating output file!" << endl;
        return;
    }

    cout << "File " + output_file_name + " created successfully.";


    if (in_file.is_open() && op_file.is_open()) //checking whether the file is open
    {
        if (file_type == "syslog")
        {
            unique_lock<mutex> lck(syslog);
            while (writing_syslog == true)
            {
                cv1.wait(lck);
            }
            string line;

            int number_of_lines = 0;

            vector<string> vec;

            while (getline(in_file, line))
            {
                vec.push_back(line);
                ++number_of_lines;
            }

            float progress = 0.0;
            int number_of_lines_read = 0;
            for (auto str : vec) { //read data from file object and put it into string.
                TCHAR username[UNLEN + 1];
                DWORD size = UNLEN + 1;
                GetUserName((TCHAR*)username, &size);

                std::time_t const now_c = std::time(NULL);
                op_file << " time " << std::put_time(std::localtime(&now_c), "%F %T") << " username " << username << "  " << str << endl;
                number_of_lines_read++;
                progress = (number_of_lines_read / (float)number_of_lines) * 100;
                std::cout << int(progress) << " %\r";
                cout << endl;
                std::cout.flush();
            }
            lck.unlock();
            cv1.notify_all();
        }

            if (file_type == "auth")
            {
                unique_lock<mutex> lck(auth);
                while (writing_auth == true)
                {
                    cv2.wait(lck);
                }
                string line;

                int number_of_lines = 0;

                vector<string> vec;

                while (getline(in_file, line))
                {
                    vec.push_back(line);
                    ++number_of_lines;
                }

                float progress = 0.0;
                int number_of_lines_read = 0;
                for (auto str : vec) { //read data from file object and put it into string.
                    TCHAR username[UNLEN + 1];
                    DWORD size = UNLEN + 1;
                    GetUserName((TCHAR*)username, &size);

                    std::time_t const now_c = std::time(NULL);
                    op_file << " time " << std::put_time(std::localtime(&now_c), "%F %T") << " username " << username << "  " << str << endl;
                    number_of_lines_read++;
                    progress = (number_of_lines_read / (float)number_of_lines) * 100;
                    std::cout << int(progress) << " %\r";
                    cout << endl;
                    std::cout.flush();
                }

                lck.unlock();
                cv2.notify_all();

            }

            op_file.close();
            in_file.close();
        }
    }

int main()
{

    string file_type1 = "syslog", output_file_name1 = "syslog_op";

    //cin >> file_type1 >> output_file_name1;   // can also take file names as input    

    string file_type2 = "auth", output_file_name2 = "auth_op";

    //cin >> file_type2 >> output_file_name2;   // can also take file names as input

    thread t1( create_file ,file_type1, output_file_name1 );
    thread t2(create_file, file_type2, output_file_name2);
    thread t3(create_file, file_type1, output_file_name1); // multiple threads writing to same output file, hence syncronization is must

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
