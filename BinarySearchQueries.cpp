/******************************************************************************************
*******************************************************************************************
*************** Online Search with Best-Price and Query-Based Predictions *****************
*******************************************************************************************
*******************************************************************************************/


// BinarySearchQueries.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <random>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>  
#include <Math.h>  
#include <algorithm>
#include <chrono>
#include <stack>          // std::stack

#include "ConversionInput.h"
using namespace std;

int main()
{
    /* setting up variables and seeding rand*/
    int n_instances = 20;
    int long_range = 200;
    int instant_length = 200;
    int col_num = 5;
    int k = 25;
    int n = (1 << k);
    srand((unsigned int) time(NULL));

    /* reading the input and setting up instances */

    cout << "please enter the complete data file name (examples are: \"BTCUSD.csv\", \"ETHUSD.csv\", \"EURUSD.csv\", and \"CADJPY.csv\" ).\n";
    cout << "The input file should be in the \"data\" folder ... \n";
    string fileName = "";
    cin >> fileName; 
    ofstream output("results/"+fileName);
    string fileNameInput = "data/" + fileName;

    cout << "file is: " << fileName << endl;
    ConversionInput* ci = new ConversionInput(fileNameInput, col_num); // read the input from file
    if (ci->file_exists == false)
    {
        cout << "The indicated file does not exist. Please try again\n";
        return 0;
    } 
    else
    {
        cout << "initializing. please wait ...";
    }
    ci->set_instances(n_instances, long_range, instant_length); // form instances (m and M, ...)
    ci->set_baseline_avg_payoffs(); // set the baseline prices
    ci->set_bst_indices(n); // set the index of the best price of each instance in the tree of the binary search with n leaves
    ci->print_summary();

    /* setting up Erroneous Binary Search structures */
    ErroneousBinarySearch* ebs = new ErroneousBinarySearch(k, n); // the tree has n leaves

    int h_n = 5;
    int* h_array = new int[h_n]; // store all possible values for H
    h_array[0] = (int)ceil((double)k * 0.1);
    h_array[1] = (int)ceil((double)k * 0.2);
    h_array[2] = (int)ceil((double)k * 0.3);
    h_array[3] = (int)ceil((double)k * 0.4);
    h_array[4] = (int)ceil((double)k * 0.5);//*/

    cout << "\n Testing RBIS for " << fileName << ". Please wait ... \n\neta\t pure \t best\t";
    for (int i = 0; i < h_n; i++)
        cout << "H=" << h_array[i] << "\t";
    cout << endl;

    int n_tests = 1000;
    for (int eta = 0; eta < 2 * k / 3; eta++)
    {
        cout << eta << "\t" << ci->avg_pure << "\t" << ci->avg_best << "\t";
        output << eta << ",\t" << ci->avg_pure << ",\t" << ci->avg_best << ",\t";
        for (int j = 0; j < h_n; j++)
        {
            int H = h_array[j];
            double avg_payoff = -1;
            if (eta <= H)
            {
                double sum = 0;
                for (int i = 0; i < n_instances; i++)
                    sum += ci->get_alg_avg_cost(k, i, H, eta, ebs, n_tests);
                avg_payoff = sum / n_instances;
                cout << avg_payoff;
                output << avg_payoff;
            }
            cout << "\t";
            output << "\t";
        }
        cout << endl;
        output << endl;
    }
    delete ebs;
    delete ci;
    std::cout << "\nProgram completed successfully\n";
    std::cout << "Find the results in \"results/\"" << fileName << "\n";
    return 0;
}