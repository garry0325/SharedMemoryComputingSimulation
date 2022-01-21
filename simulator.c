/* SUBMIT ONLY THIS FILE */
/* NAME: HAO-JYUN YANG ....... */
/* UCI ID: 95521328.......*/

// only include standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define NUM_MEMS (2048)
#define NUM_PROC (64)
#define C (1000000)
#define TERMINATE_TOLERANCE_RATE (0.002)

struct Processor {
    int memory;
    int access_time;
    int granted_access;
    int mu;
};

struct Processor* processors[NUM_PROC];
int Memory[NUM_MEMS] = {0};
int W[NUM_MEMS] = {0};

// Generate an integer from uniform distribution given a maximum value
int uniformly_random(int maximum) {
    return rand() % maximum;
}

// Generate an integer from normal distribution given a maximum value, a mean, and a standard deviation
int normally_random(int maximum, double mean, double stdev) {
    double v1 = ((double)(rand()) + 1.)/((double)(RAND_MAX) + 1.);
    double v2 = ((double)(rand()) + 1.)/((double)(RAND_MAX) + 1.);
    
    int v0 = round(cos(2 * 3.141593 * v2) * sqrt(-2. * log(v1)) * stdev + mean);
    v0 %= maximum;

    if(v0 >= maximum) v0 -= maximum;
    if(v0 < 0) v0 += maximum;

    return v0;
}

// Assign a memory to processor based on uniform distribution in the first cycle of normal distribution scenario
void initial_uniform_assignment(int processor_count, int memory_count) {
    int p;
    for(p = 0; p < processor_count; p++) {
        processors[p]->mu = uniformly_random(memory_count);
    }
}

// Initialize the Processor class instances
void initialize(int processor_count) {
    srand(time(NULL));
    
    int p;
    for(p = 0; p < processor_count; p++) {
        processors[p] = malloc(sizeof(struct Processor));
        processors[p]->memory = -1;
        processors[p]->access_time = 0;
        processors[p]->granted_access = 0;
    }
}

// Clear the parameters of Processor class in preparation for the following number of memory modules
void deinitialize(int processor_count) {
    int p;
    for(p = 0; p < processor_count; p++) {
        processors[p]->memory = -1;
        processors[p]->access_time = 0;
        processors[p]->granted_access = 0;
    }
}

// Assign memory to processors in a uniform distribution fashion
void assign_memory_to_processor_uniformly(int processor_count, int memory_count) {
    int p, assigned_memory;
    for(p = 0; p < processor_count; p++) {
        assigned_memory = uniformly_random(memory_count);
        if(Memory[assigned_memory] == 1) {
            processors[p]->access_time += 1;
        } else {
            processors[p]->granted_access += 1;
            processors[p]->memory = assigned_memory;
            Memory[assigned_memory] = 1;
        }
    }
}

// Assign memory to processors in a normal distribution fashion
void assign_memory_to_processor_normally(int processor_count, int memory_count) {
    int p, assigned_memory;
    for(p = 0; p < processor_count; p++) {
        assigned_memory = normally_random(memory_count, (double)processors[p]->mu, (double)memory_count / 6.0);
        if(Memory[assigned_memory] == 1) {
            processors[p]->access_time += 1;
        } else {
            processors[p]->granted_access += 1;
            processors[p]->memory = assigned_memory;
            Memory[assigned_memory] = 1;
        }
    }
}

// Find the next processor that has a memory assigned to it in the array of Processor
int find_next_assigned_processor(int from, int processor_count) {
    int p;
    for(p = from + 1; p < processor_count; p++) {
        if(processors[p]->memory >= 0) break;
    }
    
    return p;
}

// Move the processors that have no memory assigned to the front
void prioritize_unassigned_processor(int processor_count) {
    struct Processor* temp;
    int assigned = 0, p;
    for(p = 0; p < processor_count; p++) {
        if(processors[p]->memory == -1) {
            temp = processors[p];
            processors[p] = processors[assigned];
            processors[assigned] = temp;
            assigned = find_next_assigned_processor(assigned, processor_count);
        }
    }
}

// Update the array of cumulative average of access time
int update_W(double *avg_wait, int processor_count, int memory_count, int iteration) {
    int p;
    double sum = 0.0;
    
    for(p = 0; p < processor_count; p++) {
        if(processors[p]->granted_access > 0) {
//            sum += (double)processors[p]->access_time / (double)processors[p]->granted_access;
            sum += (double)iteration / (double)processors[p]->granted_access;

        }
    }
    
    sum = sum / processor_count;
    
    if(sum > 0.0 && avg_wait[memory_count] > 0.0) {
        if(fabs(sum - avg_wait[memory_count]) / avg_wait[memory_count] < TERMINATE_TOLERANCE_RATE) {
            return 1;
        }
    }
    
    avg_wait[memory_count] = sum;
    
    return 0;
}

// Clear the parameters of Processor class in preparation for the following cycle
void reconfigure(int processor_count, int memory_count) {
    int p, m;
    for(p = 0; p < processor_count; p++) {
        processors[p]->memory = -1;
    }
    
    for(m = 0; m < memory_count; m++) {
        Memory[m] = 0;
    }
}

void simulate(double *avg_wait, int avg_wait_l, int procs, char dist){

    // YOUR CODE GOES HERE
    int i, m;
    initialize(procs);
    
    switch (dist) {
        case 'u':
            for(m = 1; m <= NUM_MEMS; m++) {
                printf("%d\n", m);
                for(i = 0; i < C; i++) {
                    assign_memory_to_processor_uniformly(procs, m);
                    prioritize_unassigned_processor(procs);
                    reconfigure(procs, m);
                    if(update_W(avg_wait, procs, m, i) == 1) break;
                }
                deinitialize(procs);
            }
            break;
        case 'n':
            for(m = 1; m <= NUM_MEMS; m++) {
                printf("%d\n", m);
                initial_uniform_assignment(procs, m);
                for(i = 0; i < C; i++) {
                    assign_memory_to_processor_normally(procs, m);
                    prioritize_unassigned_processor(procs);
                    reconfigure(procs, m);
                    if(update_W(avg_wait, procs, m, i) == 1) break;
                }
                deinitialize(procs);
            }
            break;
        default:
            break;
    }

}
