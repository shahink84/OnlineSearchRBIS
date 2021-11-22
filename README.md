# OnlineSearchRBIS

# Online Search with Best-Price and Query-Based Predictions

This is the implementation of the Robust Binary Interval Search (RBIS) algorithm, a Query-Based prediction algorithm for the Online Search problem.

## Experiment Data
The experiment data can be download from https://eatradingacademy.com/software/forex-historical-data/.

We included `ETHUSD`, `BTCUSD`, `CADJPY`, and `EURUSD` as experiment data in the `data` folder. 

It is possible to add other data files (e.g., GBPUSD).

## Prerequisites
a c++ compiler, e.g., g++ 8.3.0 or higher

## Compiling the Project

type the followings:
g++ HeapBst.h
g++ ErroneousBinarySearch.h
g++ ConversionInput.h 
g++ BinarySearchQueries.cpp -o RBIS


## Running the Program Reproducing Results

type the following:
RBIS.exe

Follow the instructions to enter an input file name (e.g., "ETHUSD.csv"). The input file should be located in the "data" folder. 


## Experiment Result:
The resulting csv files can be found in the `results` folder.
