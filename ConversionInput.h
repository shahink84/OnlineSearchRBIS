/******************************************************************************************
*******************************************************************************************
*************** Online Search with Best-Price and Query-Based Predictions *****************
*******************************************************************************************
*******************************************************************************************/

#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include "ErroneousBinarySearch.h"


/* class ConversionInput has functions for reading the data files, maintaining problem instances, 
setting and storing baseline and RBIS prices

*/
class ConversionInput {
private:
    int input_len;
    double* all_prices;
    int start_day_jump;
    string file_name;
    bool read_file(string file_name0, int desired_column)
    {
        file_name = file_name0;
        all_prices = new double[10000]; // max number of days in the input is set to be 10000
        std::ifstream file(file_name);
        input_len = 0;
        if (file.is_open()) {
            string line;
            while (std::getline(file, line)) {
                istringstream iss(line);
                string token;
                int col = 0;
                while (std::getline(iss, token, '\t'))   // but we can specify a different one
                {
                    col++;
                    if (col == desired_column)
                    {
                        all_prices[input_len] = stod(token);
                        input_len++;
                    }
                }
            }
            file.close();
            return true;
        }
        return false;
    }


    // returns a boolearn array of size k, out of which exactly eta <=k values are false and the rest are true.
    // eta = -1 indicates that the error should be set randomly.
    bool* getAnswers(int k, int eta, int H)
    {
        if (eta < 0)
        {
            if (H + 1 < -eta)
                eta = 0;
            else
            {
                eta = rand() % ((H + 1) / (-eta));
                eta = H - eta;
            }
        }

        bool* answers = new bool[k];
        for (int i = 0; i < k; i++)
            if (i < eta)
                answers[i] = false;
            else
                answers[i] = true;
        unsigned num = (unsigned)(chrono::system_clock::now().time_since_epoch().count());
        shuffle(answers, answers + k, default_random_engine(num));
        return answers;
    }

    double get_my_value(int x_indx, int inst_indx)
    {
        x_indx = (x_indx - 1) / 2; // back to range [0,n-1]
        double a = m[inst_indx] * pow(r[inst_indx],x_indx);
        return a;
    }

public:
    double* m;
    double* M;
    double* best_rates;
    double* pure_online;
    int* bst_indx; 
    double* r; // interval exponent for each instance
    int n_instances;
    int long_range;
    int instant_length;
    bool file_exists;

    double avg_m, avg_M, avg_best, avg_pure; // baseline prices (do not change with eta)

    ConversionInput(string file_name, int desired_column)
    {
        m = M = best_rates = 0; // to be set later in set_instances
        bst_indx = 0; // to be set later in set_bst_indices.
        start_day_jump = -1; // to be set later in set_instances
        file_exists = read_file(file_name, desired_column); // read file and set all_prices
    }

    ~ConversionInput()
    {
        if (all_prices)   delete[] all_prices;
        if (m)            delete[] m;
        if (M)            delete[] M;
        if (pure_online)  delete[] pure_online;
        if (best_rates)   delete[] best_rates;
        if (bst_indx)     delete[] bst_indx;
        if (r)            delete[] r;
    }

    // create n_instance of the problem. Each instance is formed by instant_length rates. 
    // long_range indicates the range from which m and M are derived.
    void set_instances(int n_instances0, int long_range0, int instant_length0)
    {
        n_instances = n_instances0;
        long_range = long_range0;
        instant_length = instant_length0;

        m = new double[n_instances];
        M = new double[n_instances];
        best_rates = new double[n_instances];

        start_day_jump = (input_len - long_range) / n_instances;

        int start_day = 0;
        for (int inst_indx = 0; inst_indx < n_instances; inst_indx++)
        {
            double min = 1000000;
            double max = -1;
            for (int i = 0; i < long_range; i++)
            {
                double next = all_prices[start_day + i];
                if (next < min)
                    min = next;
                if (next > max)
                    max = next;
            }
            m[inst_indx] = min;
            M[inst_indx] = max;

            max = -1;
            for (int i = 0; i < instant_length; i++)
            {
                double next = all_prices[start_day + i];
                if (next > max)
                    max = next;
            }

            best_rates[inst_indx] = max;
            start_day += start_day_jump;
        }

    }

    // given a reservation price, output the payoff for the instance indexed "inst_index" 
    double get_reservation_payoff(double reservation_price, int inst_index)
    {
        int start_day = inst_index * start_day_jump; 
        for (int i = 0; i < instant_length; i++)
        {
            double price = all_prices[start_day + i];
            if (price >= reservation_price)
                return price;
        }
        for (int i = 0; i < instant_length; i++)
        {
            double price = all_prices[start_day + i];
            cout << price << "\t";
        }
        return all_prices[start_day + instant_length - 1]; // force trade at last day
    }

    // set average payoffs for baseline algorithms (those that are independent of error).
    void set_baseline_avg_payoffs()
    {
        double sum_m, sum_M, sum_best, sum_pure;
        sum_m = sum_M = sum_best = sum_pure = 0;
        
        pure_online = new double[n_instances];
        for (int i = 0; i < n_instances; i++)
        {
            sum_m += m[i];
            sum_M += M[i];
            sum_best += best_rates[i];
     
            double res_p = sqrt(m[i] * M[i]);
            pure_online[i] = get_reservation_payoff(res_p, i);
            sum_pure += pure_online[i];
        }

        avg_m = (double)(sum_m) / n_instances;
        avg_M = (double)(sum_M) / n_instances;
        avg_best = (double)(sum_best) / n_instances;
        avg_pure = (double)(sum_pure) / n_instances;
    }

    /* For each instance i, divide the price range [m,M] into n intervals, as descriebed in the paper.
     Find the best price for each instance, and set the index of the interval it belongs to in bst_indx[i]*/
    void set_bst_indices(int n)
    {
        bst_indx = new int[n_instances];
        r = new double[n_instances];

        for (int i = 0; i < n_instances; i++)
            r[i] = bst_indx[i] = -1;
        
        for (int i = 0; i < n_instances; i++)
        {
            r[i] = pow((double)M[i] / m[i], 1.0 / (double)(n));
            int indx = 0;
            double a = m[i];
            while (bst_indx[i] == -1 && a < M[i])
            {
                if (a * r[i] > best_rates[i])
                    bst_indx[i] = indx;
                indx++;
                a *= r[i];
            }
        }

        // indices should be odd (even numbers are internal nodes and leaves are odd in the tree)
        for (int i = 0; i < n_instances; i++)
            bst_indx[i] = bst_indx[i] * 2 + 1; 
    }
    
    // print a summary of the data
    void print_summary()
    {
        cout << "\n____________________________\n";
        cout << "printing the summary for " << file_name << endl;
        cout << "input length is: " << input_len << endl;
        cout << "number of instances: " << n_instances << endl;
        cout << "instant lengths: " << instant_length << endl;
        cout << "long_range (for setting m and M): " << long_range << endl;
        cout << "start_day_jump: " << start_day_jump << endl;
        cout << "-----\n";
        cout << "average m: " << avg_m << endl;
        cout << "average M: " << avg_M << endl;
        cout << "average best_price: " << avg_best << endl;
        cout << "average pure_online: " << avg_pure << endl;
        cout << "-----\n inst \t m \t M \t Best \t pure-online\n";
        for (int i = 0; i < n_instances; i++)
            cout << i << "\t" << m[i] << "\t" << M[i] << "\t" << best_rates[i] << "\t" << pure_online[i]  << endl;
        cout << "printing summary ends\n";
        cout << "\n____________________________\n";
    }

    // for instance i, set the algorithm's average cost,
    double get_alg_avg_cost (int k, int i, int H, int eta, ErroneousBinarySearch* ebs, int n_tests)
    {
        int x = bst_indx[i]; // we are looking for this index in the array
        double sum = 0;
        for (int t = 0; t < n_tests; t++)
        {
            bool* answers = getAnswers(k, eta, H);
            int parent_indx = ebs->bs_limited_query(x, answers, H, false);
            int low_indx = ebs->T->getLeftmostLeaf(parent_indx);
            int high_indx = ebs->T->getRightmostLeaf(parent_indx);
            double low = get_my_value(low_indx, i);
            double high = get_my_value(high_indx, i);
            double res_price = sqrt(low * high);
            sum += get_reservation_payoff(low, i);
            delete[] answers;
        }
        return sum / n_tests;
    }
};