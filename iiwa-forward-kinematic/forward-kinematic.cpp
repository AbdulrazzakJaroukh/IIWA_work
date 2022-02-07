#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include <fstream>
#include <vector>
#include <string>
#include <utility> // std::pair
#include <string>
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream


using namespace std;
Eigen::Matrix4d T0_N(vector<double> values);
Eigen::Matrix4d evalDHMatrix(double a, double d, double alpha, double theta);
void write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<double>>> dataset);
std::vector<std::pair<std::string, std::vector<double>>> read_csv(std::string filename);


void write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<double>>> dataset)
{
    // Make a CSV file with one or more columns of integer values
    // Each column of data is represented by the pair <column name, column data>
    //   as std::pair<std::string, std::vector<int>>
    // The dataset is represented as a vector of these columns
    // Note that all columns should be the same size

    // Create an output filestream object
    std::ofstream myFile(filename);

    // Send column names to the stream
    for(int j = 0; j < dataset.size(); ++j)
    {
        myFile << dataset.at(j).first;
        if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
    }
    myFile << "\n";

    // Send data to the stream
    for(int i = 0; i < dataset.at(0).second.size(); ++i)
    {
        for(int j = 0; j < dataset.size(); ++j)
        {
            myFile << dataset.at(j).second.at(i);
            if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
        }
        myFile << "\n";
    }

    // Close the file
    myFile.close();
}

std::vector<std::pair<std::string, std::vector<double>>> read_csv(std::string filename)
{
    // Reads a CSV file into a vector of <string, vector<int>> pairs where
    // each pair represents <column name, column values>

    // Create a vector of <string, int vector> pairs to store the result
    std::vector<std::pair<std::string, std::vector<double>>> result;

    //std::ifstream myFile;
    //std::string File1Name = ("/home/abdul/moveit_ws/src/iiwa/moveit_iiwa/doc/move_group_interface/src/Results/"+ PlannerId +"-iiwa.txt");
    //myFile.open(filename);
    // Create an input filestream
    std::ifstream myFile(filename);

    // Make sure the file is open
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    // Helper vars
    std::string line, colname;
    double val;

    // Read the column names
    if(myFile.good())
    {
        // Extract the first line in the file
        std::getline(myFile, line);

        // Create a stringstream from line
        std::stringstream ss(line);

        // Extract each column name
        while(std::getline(ss, colname, ',')){

            // Initialize and add <colname, int vector> pairs to result
            result.push_back({colname, std::vector<double> {}});
        }
    }

    // Read data, line by line
    while(std::getline(myFile, line))
    {
        // Create a stringstream of the current line
        std::stringstream ss(line);

        // Keep track of the current column index
        int colIdx = 0;

        // Extract each integer
        while(ss >> val){

            // Add the current integer to the 'colIdx' column's values vector
            result.at(colIdx).second.push_back(val);

            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();

            // Increment the column index
            colIdx++;
        }
    }

    // Close file
    myFile.close();

    return result;
}

Eigen::Matrix4d T0_N(vector<double> values)
{
    Eigen::Matrix4d m;
    Eigen::Matrix4d G_sL0, G_01, G_12, G_23, G_34, G_45, G_56, G_67;

    G_01 = evalDHMatrix(0.0,         0.34,         -M_PI/2,         values.at(0));
    G_12 = evalDHMatrix(0.0,         0.0,          M_PI/2,          values.at(1));
    G_23 = evalDHMatrix(0.0,         0.4,          -M_PI/2,         values.at(2));
    G_34 = evalDHMatrix(0.0,         0.0,          M_PI/2,          values.at(3));
    G_45 = evalDHMatrix(0.0,         0.4,          -M_PI/2,         values.at(4));
    G_56 = evalDHMatrix(0.0,         0.0,          M_PI/2,          values.at(5));
    G_67 = evalDHMatrix(0.0,         0.126,        0.0,             values.at(6));

    m = G_01*G_12*G_23*G_34*G_45*G_56*G_67;
    return m;

}

Eigen::Matrix4d evalDHMatrix(double a, double d, double alpha, double theta)
{
    Eigen::Matrix4d evalDH_arr;
    evalDH_arr << cos(theta), -sin(theta) * cos(alpha), sin(theta) * sin(alpha) , cos(theta) * a,
                sin(theta), cos(theta) * cos(alpha), -cos(theta) * sin(alpha), sin(theta) * a,
                0        , sin(alpha)           , cos(alpha)           , d          ,
                0        , 0                   ,         0           , 1          ;
    return evalDH_arr;
}


int main()
{
    std::vector<std::pair<std::string, std::vector<double>>> JointsValues = read_csv("/home/abdul//ws/src/IIWA_work/Input/iiwa-trajectory-joint-samples-without-tunning.csv");

    std::vector<std::pair<std::string, std::vector<double>>> CartesianCoordinates;

    CartesianCoordinates.push_back({"X", std::vector<double>{}});
    CartesianCoordinates.push_back({"Y", std::vector<double>{}});
    CartesianCoordinates.push_back({"Z", std::vector<double>{}});


    for (int j=0; j<JointsValues.at(0).second.size(); j++)
    {
        std:vector<double> SinglePos{};
        Eigen::Matrix4d Tr;
        for (int i = 0; i < JointsValues.size(); i++)
            SinglePos.push_back(JointsValues.at(i).second.at(j));

        Tr = T0_N(SinglePos);
        CartesianCoordinates.at(0).second.push_back(Tr(0,3));
        CartesianCoordinates.at(1).second.push_back(Tr(1,3));
        CartesianCoordinates.at(2).second.push_back(Tr(2,3));
    }
    write_csv("/home/abdul//ws/src/IIWA_work/Output/iiwa-trajectory-cartesian-samples-without-tunning.csv", CartesianCoordinates);

    return 0;
}
