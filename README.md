# LKH3
Customized version of Keld Helsgaun's LKH3 algorithm for vehicle routing problems. 

__*All rights reserved to the original author.*__ 

The original code is available [here](http://webhotel4.ruc.dk/~keld/research/LKH-3), along with a large dataset of instances for VRP problems supported by the LKH-3 algorithm. 
 
In this version, I tried to improve the performances of the LKH-3 in terms of computing efficiency, maintaining, when possible, the search trajectory with respect to the original code. 

The main modifications introduced regarded the *Penalty* function of some of the variants (namely CVRP and CVRPTW), the caching system, and the *Flip* function for asymmetric problems.

### CVRP and CVRPTW Penalty functions
The main idea has been to restrict the exploration of the TSP-tour representation to only the routes touched by the current r-opt move.
Some data needs to be kept updated between calls, but this O(n) update is needed only when an improving solution is found (which happens rarely), speeding-up the common "rejecting case".

For the *Penalty* function, CVRP and CVRPTW are used as an example, however, the same techniques can be extended also to other mTSP-like problems.

### Caching Systems
The cache-check is move in a small function prologue (another smaller function called before the original one) ready to be inlined by the compiler.

### Linear Flip
For problems that are represented using the ATSP->TSP transformation, no actual flip can happen. Exploiting this limitation of the r-opt moves in asymmetric problems, the Flip function can be made O(1), with the addition of a O(n) "update step" that needs to be called when an actual improvement is found (which is rare).
Note that, switching from two-level tree representation to the single-level one affects the search trajectory.

---

*Part of the development has been carried out during the work for the Set-Partitioning based local-search heuristic realized in joint work with Emilio Bendotti and Matteo Fischetti.*
***{insert paper data when it will be available}***

---

## Original README.txt content:

LKH is an implementation of the Lin-Kernighan traveling salesman heuristic.

The code is distributed for research use. The author reserves all rights to 
the code.

### INSTRUCTIONS FOR INSTALLATION: (Version 3.0.6 - May 2019)

The software is available in gzipped tar format:

	LKH-3.0.6.tgz	(approximately 2.3 MB)

Download the software and execute the following UNIX commands:

  	tar xvfz LKH-3.0.6.tgz
   	cd LKH-3.0.5
	make

An executable file called LKH will now be available in the directory LKH-3.0.6.

To test the installation run the program by typing ./LKH pr2392.par. 
Then press return. The program should now solve a TSP instance with 2392 nodes.

For testing the installation on an mTSP problem, type ./LKH whizzkids96.par.
Then press return.

A two-level tree is used as the default tour representation. 
A three-level tree representation may be used instead by compiling the
source code with the compiler option 

	-DTHREE_LEVEL_TREE

Just edit the first line in SRC/Makefile and execute the commands

	make clean
	make

### CHANGES IN LKH-3.0.6:

Added code for solving the Steiner traveling salesman problem (STTSP). 
New keyword

    REQUIRED_NODES_SECTION

### CHANGES IN LKH-3.0.5:

Added code for solving the open close multiple traveling salesman problem (OCMTSP). 
New keyword

    EXTERNAL_SALESMEN


### CHANGES IN LKH-3.0.4:

Added code for solving the colored traveling saleman problem (CTSP). 
The node coloring is described in a

    CTSP_SET_SECTION

New initial tour algorithm: CTSP

Added code solving the minimum latency problem (MLP).

### CHANGES IN VERSION 3.0.3:

Candidate sets may now be created by means of POPMUSIC by giving the following
specification in the parameter file for LKH:

	CANDIDATE_SET_TYPE = POPMUSIC

The value of the parameter MAX_CANDIDATES is used to trim the candidate set.
There are, however, some other POPMUSIC related parameters. If not specified,
they will take their default values. These parameters are:

    POPMUSIC_SAMPLE_SIZE = <int>  
    Sample size.
    Default: 10.

    POPMUSIC_SOLUTIONS = <int> 
    Number of solutions to generate.
    Default: 50.

    POPMUSIC_MAX_NEIGHBORS = <int>
    Maximum number of nearest neighbors used as candidates in iterated 3-opt for
    POPMUSIC.
    Default: 5.

    POPMUSIC_TRIALS = <int>
    Number of trials used in iterated 3-opt for POPMUSIC. 
    If the value is zero, the number of trials is the size of the subpath
    to be optimized.
    Default: 1.

    POPMUSIC_INITIAL_TOUR = { YES | NO }
    Specifies whether the first generated POPMUSIC tour is used as
    initial tour for Lin-Kernighan.
    Default: NO.

### CHANGES IN VERSION 3.0.2:

Tours may now be recombined by GPX2 (Generalized Partition Crossover 2) 
instead of IPT (Iterative Partial Transcription). 

GPX2 is used by giving the following specification in the parameter file:

	RECOMBINATION = GPX2

The possible settings are:

	RECOMBINATION = { IPT | GPX2 } 

IPT is default.

	
### CHANGES IN VERSION 3.0.1:

    New problem type: TSPDL (traveling salesman with draft limits)

### NEW IN VERSION 3.0:
	
New parameter keywords:

	BWTSP = <integer> <integer> [ <integer> ]
	DEPOT = <integer>
	MAKESPAN = { YES | NO }	MTSP_MIN_SIZE = <integer>
	MTSP_MAX_SIZE = <integer>
	MTSP_OBJECTIVE = [ MINMAX | MINMAX_SIZE | MINSUM ]
	MTSP_RESULT_FILE = <string>
	SINTEF_RESULT_FILE = <string>
	SALESMEN = <integer>
	SCALE = <integer>
	VEHICLES : <integer>

	New initial tour algorithms:
	CVRPR
	MTSP
	SOP 

New TSPLIB format keywords:

The specification part: 

	CAPACITY : <integer>
	DEMAND_DIMENSION : <integer>
	DISTANCE : <real>
	GRID_SIZE : <real>
	RISK_THRESHOLD : <integer>
	SALESMEN : <integer>
	SCALE : <integer> 
	SERVICE_TIME_SECTION 
	VEHICLES : <integer>

	New edge weight types:	
	EXACT_2D
	EXACT_3D 
	FLOOR_2D
	FLOOR_3D
	TOR_2D
	TOR_3D 

The data part: 

	BACKHAUL_SECTION
	DEMAND_SECTION
	DEPOT_SECTION
	PICKUP_AND_DELIVERY_SECTION
	TIME_WINDOW_SECTION
