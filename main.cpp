#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>


using namespace std;

bool convert_time_to_seconds(std::string time, int *seconds)
{
    int h,m;
    if(time.find("am") != std::string::npos)
    {
        //Removes am from the string. It assumes rest of the format is compliant
        std::replace_if(time.begin(), time.end(), ::isalpha, ' ');

        // check if sscanf returns 2 for the 2 variables. Makes sure time format is in order
        if (sscanf(time.c_str(), "%d:%d", &h, &m) == 2)
            *seconds = h *3600 + m*60;
        else
            return false;
    }
    else
    {
        std::replace_if(time.begin(), time.end(), ::isalpha, ' ');

        // If it's pm, add additional 12 hrs worth of seconds.
        // Catch is if it's 12PM we should not add 12hrs worth of seconds. Use modulus.
        // Below code with (h%12) takes care of the time at exact 12 pm
        if (sscanf(time.c_str(), "%d:%d", &h, &m) == 2)
            *seconds = (h%12)*3600 + m*60 + 12*3600;
        else
            return false;
    }

    return true;
}

// This function creates and returns a sequential time list by parsing the file supplied
std::vector<std::string> create_time_list(const std::string &file_name)
{

    string line;
    std::vector<std::string> time_list;
    if(file_name == "")
        return time_list;

    std::ifstream infile(file_name.c_str());

    //scan line by line and populate the time_list sequentially from the supplied time. Below code contains few string manipualtion operators.
    while (std::getline(infile, line))
    {
        //Skip the line containing start or end
        if(!line.find("start") || !line.find("end"))
            continue;

        //Extract the substrings by excluding the comma and build a linear sequential list of time.
        string token;
        std::istringstream tokenStream(line);
        while (std::getline(tokenStream, token,','))
            time_list.push_back(token);
    }

//    for(int i = 0; i < time_list.size(); i++)
//        std::cout << "time_list : " << time_list[i] << std::endl;

    return time_list;
}

int identify_conflicts(std::vector<string> time_list, bool out_file)
{
    int count=0, count1=0, h, m;
    std::ofstream out("output.txt");
    // Once the time list is ready, iterate through it
    for (std::vector<std::string>::const_iterator i = time_list.begin(); i != time_list.end(); ++i)
    {
        int start_secs, end_secs;
        if(!convert_time_to_seconds(*i, &start_secs))
        {
            std::cout << "Supplied data is not in expected format" << std::endl;
            return false;
        }

        //Advance the array
        ++i;
        if(!convert_time_to_seconds(*i, &end_secs))
        {
            std::cout << "Supplied data is not in expected format" << std::endl;
            return false;
        }

        count1=0;
        for (std::vector<std::string>::const_iterator j = time_list.begin(); j != time_list.end(); ++j)
        {
            int start_secs1, end_secs1;
            if(!convert_time_to_seconds(*j, &start_secs1))
            {
                std::cout << "Supplied data is not in expected format" << std::endl;
                return false;
            }

            //Advance the array
            ++j;
            if(!convert_time_to_seconds(*j, &end_secs1))
            {
                std::cout << "Supplied data is not in expected format" << std::endl;
                return false;
            }

            if(((start_secs < start_secs1) && (start_secs1 < end_secs)) || ((start_secs < end_secs1) && (end_secs1 < end_secs)))
            {

                std::string str = "Schedule " +  time_list[count*2] + "," + time_list[count*2+1] + " conflicts with "
                        + time_list[count1*2] + "," + time_list[count1*2+1];
                if(out_file)
                   out << str << std::endl;
                else
                    std::cout << str << std::endl;
            }

            count1++;
        }

        count++;
    }

    out.close();
}

bool findExactStringMatch(const std::string& arg, const std::string & search) {

        size_t pos = arg.find(search);
        return (pos != std::string::npos) && (pos==0 || isspace(arg[pos-1]));
}

int main(int argc, char *argv[])
{
    std::vector<string> time_list;
    bool output_to_file = false;

    // Loop over the argument list
    for (int i = 0; i< argc; i++) {
        if (findExactStringMatch("-input", argv[i])) {
            i++;
            time_list = create_time_list(argv[i]);
         }
        else if (findExactStringMatch("-test", argv[i])) {
            output_to_file = true;
         }
    }

    //Unable to extract the time list
    if(time_list.size() == 0)
    {
        std::cout << "Unable to parse the file" << std::endl;
        return false;
    }

    identify_conflicts(time_list, output_to_file);
    return 0;
}
