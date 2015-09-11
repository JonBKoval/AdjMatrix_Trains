#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <string>

//===============================================================
//   PROGRAM:  Assignment 3
//   AUTHOR :  Jonathan Koval
//   DUE    :  4/30/2014
//   DESCRIPTION:
//          This program uses graphs to model the shortest path
//		between two points or nodes. Models a railroad system
//===============================================================

using namespace std;

// GLOBAL CONSTANT  =============================================
const int MAX_SIZE = 100;
const char STATIONS[] = "stations.dat";
const char TRAINS[] = "trains.dat";

// GLOBAL TYPE DEF  =============================================
struct TRAIN
{
	bool exists;
	int departure;
	int arrival;
	int departure_station;
	int arrival_station;
	int traveled;
};

struct STATION
{
	int number;
	string name;
};

 
// ====================== PROTOTYPES ============================
int main_menu();
void setup_matrix(TRAIN adj_index[MAX_SIZE][MAX_SIZE]);
void list_stations(STATION station_lookup[MAX_SIZE],int count);
void shortest_overall();
int build_station_lookup(STATION station_lookup[MAX_SIZE]);
string convert_time(const int minutes);
void convert_hours(int & minutes, int & hours);
int ret_station_number(STATION station_lookup[MAX_SIZE], string station);
string lookup_station(STATION station_lookup[MAX_SIZE],int station_number);
int count_station_jumps(int midpoints[10000]);
int total_time_traveled(int midpoints[10000], const int count, TRAIN adj_index[MAX_SIZE][MAX_SIZE]);
void trains_taken(int midpoints[10000], const int count, TRAIN adj_index[MAX_SIZE][MAX_SIZE],
				  STATION station_lookup[MAX_SIZE]);
void set_arriv_dept_station(int midpoints[10000],const int dept_station,const int arriv_station);
void path(int P[MAX_SIZE][MAX_SIZE],int midpoints[10000], int i, int j, int count );
void floyd_shortest(int A[MAX_SIZE][MAX_SIZE],TRAIN adj_index[MAX_SIZE][MAX_SIZE],
					int P[MAX_SIZE][MAX_SIZE]);
void shortest_time(TRAIN adj_index[MAX_SIZE][MAX_SIZE],STATION station_lookup[MAX_SIZE],
					const int dept_station,const int arriv_station);
//===============================================================

 
// ======================= MAIN =================================
int main(void)
{
	int station_count = 0;
	int departure_station = 0;
	int arrival_station = 0;
	TRAIN adj_index [MAX_SIZE][MAX_SIZE];
	STATION station_lookup [MAX_SIZE];
	string station_name;

	station_count = build_station_lookup(station_lookup);
	setup_matrix(adj_index);

	int choice = 0;
	do
	{
		choice = main_menu();
		switch(choice)
		{		
		case 1:  //List stations and numbers
			list_stations(station_lookup, station_count);
			break;
		
		case 2:  //Find shortest travel time on trains
			cout << "Enter the station you will depart from: ";
			cin.ignore(1);  //Ignores the new line character sitting in the buffer
			getline(cin, station_name, '\n');
			departure_station = ret_station_number(station_lookup, station_name);

			cout << "Enter the station you wish to arrive at: ";
			getline(cin, station_name);
			arrival_station = ret_station_number(station_lookup, station_name);

			shortest_time(adj_index,station_lookup,departure_station,arrival_station);
			break;
		
		case 3:  //Find shortest time overall
			shortest_overall();
			break;

		case 4:  //Exit
			exit(1);
			break;
		
		default:
			cout << "Invalid choice, please select again." << endl << endl; 
			break;
		}
	}while(choice != 4);

	return 0;
}


// ================== FUNCTION DEFINITIONS ======================

//Main driver for the program returns which option is selected
int main_menu()
{
	int choice = 0;

	cout << "Main Menu:\n" << endl;
	cout << "\t1.\tList Stations and #s" << endl;
	cout << "\t2.\tFind Shortest Travel Time on Trains" << endl;
	cout << "\t3.\tFind Shortest Time Overall(Not implemented)" << endl;
	cout << "\t4.\tExit" << endl;
	cout << "\n\nSelect from the above options:";

	cin >> choice;

	cout << endl;

	return choice;
}

// Sets up the adjacency matrix for the trains
void setup_matrix(TRAIN adj_index[MAX_SIZE][MAX_SIZE])
{
	ifstream fin;
	fin.clear();
	fin.open(TRAINS, ios::in);
	TRAIN temp;

	if (!fin.is_open())
		exit(1);

	int i = 0;
	int j = 0;

	//Initialize all stations to not exist
	for(i = 0; i < 100; i++)
	{
		for(j = 0; j < 100; j++)
		{
			adj_index[i][j].exists = 0;
			adj_index[i][j].traveled = 50000;
			adj_index[i][j].departure = 0;
			adj_index[i][j].arrival = 50000;
			adj_index[i][j].arrival_station = j;
			adj_index[i][j].departure_station = i;
		}
	}

	while (!fin.eof())
	{
		fin >> temp.departure_station;
		fin >> temp.arrival_station;
		fin >> temp.departure;
		fin >> temp.arrival;
		temp.exists = 1;
		temp.traveled = temp.arrival - temp.departure;

		for(i = 0; i < MAX_SIZE; i++)
		{
			for(j = 0; j < MAX_SIZE; j++)
			{
				// Creates a station in the index
				if( temp.departure_station == i && temp.arrival_station == j)
				{
					adj_index[i][j] = temp;
				}
				// Creates a blank station setting exists to 0
			}
		}
	}
	//testing function
/*	for(i = 0; i < 100; i++)
	{
		for(j = 0; j < 100; j++)
		{
			if(adj_index[i][j].exists == 1)
			{
				cout << "DS#: " << adj_index[i][j].departure_station << endl;
				cout << "AS#: " << adj_index[i][j].arrival_station << endl;
				cout << "DT: " << adj_index[i][j].departure << endl;
				cout << "AT: " << adj_index[i][j].arrival << endl;
				cout << "TT: " << adj_index[i][j].traveled << endl << endl;
			}
		}
	}
	*/
	fin.close();
}

//Converts raw minutes into hours and minutes AM/PM
string convert_time(const int minutes)
{
	string time = "";
	time.clear();
	int mins = minutes;
	int hours = 0;
	bool am_pm_switch = 0;

	//Handles when am and pm switch on the hour instead of 1 minute after
	if ( mins == 1440 )
		am_pm_switch = 1;

	if (mins == 0 )
		am_pm_switch = 1;

	while(mins >= 60) //calculates hours and minutes
	{
		hours++;
		mins = mins - 60;
	}

	if(hours >= 12) //logic to create string of characters representing time
	{
		hours = hours - 12;
		if(hours == 0)
			hours = 12;

		time = to_string(hours);
		time = time + ":";
		if(mins < 10)
			time = time + "0";
		time = time + to_string(mins);
		if(am_pm_switch == 0)
			time = time + " PM";
		else
			time = time + " AM";
	}
	else
	{
		if(hours == 0)
			hours = 12;
		time = to_string(hours);
		time = time + ":";
		if(mins < 10)
			time = time + "0";
		time = time + to_string(mins);
		if(am_pm_switch == 0)
			time = time + " AM";
		else
			time = time + " PM";
	}
	
	return time;
}

//Converts minutes to minutes and hours for reporting
void convert_hours(int & minutes, int & hours)
{
	hours = 0;

	while(minutes >= 60 ) //calculates hours and minutes
	{
		hours++;
		minutes = minutes - 60;
	}
}

//Builds the lookup table for the stations returns the amount of stations
int build_station_lookup(STATION station_lookup[MAX_SIZE])
{
	ifstream fin;
	fin.open(STATIONS, ios::in);
	int count = 0;
	string temp;
	if (fin.is_open())
	{
		for(int i = 0; i < MAX_SIZE; i++)
		{
			if (!fin.eof())
			{
				fin >> station_lookup[i].number; //Reads a number into the lookup array
				while((fin.peek() != '\n') && !fin.eof())  // Needs to be changed to /r for cygwin
				{
					fin >> station_lookup[i].name;   //reads the name into the lookup array
					
					while(fin.peek() == ' ') // handles the case of stations with spaces in their name
					{
						fin >> temp;
						station_lookup[i].name = station_lookup[i].name + ' ' + temp;
					}
				}
				count++;
			}
			else  // sets all the unused spots in the array
			{
				station_lookup[i].number = -1;
				station_lookup[i].name = "";
			}
		}
	}
	else
	{
		cout << "\nError Opening " << STATIONS << " File\n" << endl;
		return -1;
	}
	fin.close();
	return count;
}

//Lists all the stations and their corresponding numbers
void list_stations(STATION station_lookup [MAX_SIZE],int count)
{
	for(int j = 0; j < count; j++)
	{
		cout << "Station number: " << station_lookup[j].number << endl;
		cout << "Station name: " << station_lookup[j].name << endl << endl;
	}
}

//Find shortest travel time on trains
void shortest_time(TRAIN adj_index[MAX_SIZE][MAX_SIZE],STATION station_lookup[MAX_SIZE],
		const int dept_station,const int arriv_station)
{
	int A[MAX_SIZE][MAX_SIZE];
	int P[MAX_SIZE][MAX_SIZE];
	int midpoints[10000];
	int hours = 0;
	int minutes = 0;
	int count = 9999; //Variable for midpoint array reused for count of stations
	string station_Depart = "";
	string station_Arrive = "";
	//Initialize variables
	for (int i = 0; i < 10000; i++)
	{
		midpoints[i] = -1;
	}
	//Sets the arrival and departure station string for output
	station_Depart = lookup_station(station_lookup,dept_station);
	station_Arrive = lookup_station(station_lookup,arriv_station);

	//Run through Floyds Shortest algorithm
	floyd_shortest(A,adj_index,P);
	path(P, midpoints, dept_station, arriv_station, count);
	set_arriv_dept_station(midpoints,dept_station,arriv_station);
	count = count_station_jumps(midpoints);
	minutes = total_time_traveled(midpoints,count,adj_index);
	convert_hours(minutes, hours);

	if( hours > 800)
	{
		cout << endl << "No route exists between the two stations" << endl;
		return;
	}

	cout << "\nREPORT 1: Shortest time riding on trains.\n" << endl;
	cout << "To go from " << station_Depart << " to " << station_Arrive;
	cout << " you will need to ride on trains" << endl;
	cout << hours << " hours and " << minutes << " minutes." << endl;

	cout << "\nYou will take the following trains:\n" << endl;
	trains_taken(midpoints, count, adj_index, station_lookup);
	cout << "\n\nEND OF REPORT" << endl;
}

//Finds the shortest time overall
void shortest_overall()
{
	cout << "\nThis feature has not been implemented" << endl;
}

//passed in a string returns the station number if found
//returns -1 if station does not exist
int ret_station_number(STATION station_lookup[MAX_SIZE], string station)
{
	int station_number = -1;

	for(int i = 0; i < MAX_SIZE; i++)
	{
		if(station_lookup[i].name == station)
		{
			return station_lookup[i].number;
		}
	}
	return -1; 
}

//Floyd Algorithm
void floyd_shortest(int A[MAX_SIZE][MAX_SIZE],TRAIN adj_index[MAX_SIZE][MAX_SIZE],
					int P[MAX_SIZE][MAX_SIZE])
{
	int i = 0,j = 0,k = 0;

	for (i = 0; i < MAX_SIZE; i++)
	{
		for (j = 0; j < MAX_SIZE; j++)
		{
			A[i][j] = adj_index[i][j].traveled;
			P[i][j] = -1;
		}
	}

	for (i = 0; i < MAX_SIZE; i++)
	{
		A[i][i] = 0;
	}

	for (k = 0; k < MAX_SIZE; k++)
	{
		for (i = 0; i < MAX_SIZE; i++)
		{
			for (j = 0; j < MAX_SIZE; j++)
			{
				if (A[i][k] + A[k][j] < A[i][j])
				{
					//Sets new distance
					A[i][j] = A[i][k] + A[k][j];
					
					// K is the station inbetween the stations you just checked
					P[i][j] = k;
				}
			}
		}
	}
}

//Path tracking
void path( int P[MAX_SIZE][MAX_SIZE],int midpoints[10000], int i, int j, int count)
{
	int k = 0;
	count--;

	// Check to make sure array index is valid
	if ( i > -1 && i < MAX_SIZE && j > -1 && j < MAX_SIZE )
		k = P[i][j];
	
	if( k == -1 )
		return;

	path( P, midpoints, i, k, count);

	midpoints[count] = k;

	path( P, midpoints, k, j, count);
}

//Looks up the station and returns its name
string lookup_station(STATION station_lookup[MAX_SIZE], int station_number)
{
	for(int i = 0; i < MAX_SIZE; i++)
	{
		if(station_lookup[i].number == station_number)
			return station_lookup[i].name;
	}
	return "";
}

//Sets up the array of stations traveled between with arrival and departure
//	stations at the beginning and end of array
void set_arriv_dept_station(int midpoints[10000],const int dept_station,const int arriv_station)
{
	bool placed = 0;
	midpoints[9999] = arriv_station; // Sets the end of the array to final station

	for (int i = 0; i < 10000; i++)
	{
		if ( midpoints[i+1] > -1 && placed != 1)
		{
			midpoints[i] = dept_station;  // Sets the first station
			placed = 1;
		}
	}
}

//Counts how many stations you passed through
int count_station_jumps(int midpoints[10000])
{
	int count = 0;
	for (int i = 0; i < 10000; i++)
	{
		if ( midpoints[i] != -1)
		{
			count++;
		}
	}
	return count;
}

//Finds the total time traveled on all trains traveled and returns it
int total_time_traveled(int midpoints[10000], const int count, TRAIN adj_index[MAX_SIZE][MAX_SIZE])
{
	int total_time = 0;
	int start_real_values = 0;
	int dept_station = 0;
	int arriv_station = 0;
	bool found = 0;
	for (int i = 0; i < 10000; i++)
	{
		if ( midpoints[i] != -1 && found != 1)
		{
			start_real_values = i;
			found = 1;
		}
	}
	for (int j = start_real_values; j < 9999; j++)
	{
		dept_station = midpoints[j];
		arriv_station = midpoints[j+1];
		total_time = adj_index[dept_station][arriv_station].traveled + total_time;
	}
	return total_time;
}

//Prints out trains taken and their departure and arrival times
void trains_taken(int midpoints[10000], const int count, TRAIN adj_index[MAX_SIZE][MAX_SIZE],
				  STATION station_lookup[MAX_SIZE])
{
	int start_real_values = 0;
	int dept_station = 0;
	int arriv_station = 0;
	string departure = "";
	string arrival = "";
	string dept_time = "";
	string arriv_time = "";
	bool found = 0;
	for (int i = 0; i < 10000; i++)
	{
		if ( midpoints[i] != -1 && found != 1)
		{
			start_real_values = i;
			found = 1;
		}
	}
	for (int j = start_real_values; j < 9999; j++)
	{
		dept_station = midpoints[j];
		arriv_station = midpoints[j+1];
		departure = lookup_station(station_lookup, dept_station);
		arrival = lookup_station(station_lookup, arriv_station);
		dept_time = convert_time(adj_index[dept_station][arriv_station].departure);
		arriv_time = convert_time(adj_index[dept_station][arriv_station].arrival);
		cout << "Leave " << departure << " at " << dept_time;
		cout << " and arrive at " << arrival << " at " << arriv_time;
		cout << endl;
	}
}
