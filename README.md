# Parallel Programming — Course Projects

> **Author:** Subham Gaurav (fd0003361 / 1564925)
> **Course:** Parallel Programming — AI5085 (SoSe 2025)

A collection of parallel-programming projects covering three major paradigms:
- **OpenMP** — shared-memory parallelism (tasks, sections, reduction)
- **POSIX Threads (`pthreads`)** — low-level multi-threading
- **MPI (Message Passing Interface)** — distributed-memory parallelism

All experiments were executed on a university HPC cluster managed by **SLURM**.

---

## Repository Structure

```
Parallel_Programming/
│
├── ── OpenMP / POSIX Threads ──
├── Sum_Serial.c                        # Baseline serial sum (N = 100,000,000)
├── Sum_Threads.c                       # Parallel sum using 10 POSIX threads
├── sumwithopenmp.c                     # Parallel sum — OpenMP reduction (fastest)
├── sumwithopenmp_Sections.c            # Parallel sum — OpenMP sections + critical
├── sumwithopenmp_tasks.c               # Parallel sum — OpenMP tasks
│
├── QuickSort_Serial.c                  # Baseline serial QuickSort (N = 500,000)
├── QuickSort_Parallel.c                # Parallel QuickSort — OpenMP tasks w/ cutoff
├── QuickSort_Parallel_Sections.c       # Parallel QuickSort — OpenMP sections
├── QuickSort_Parallel_With_Overhead.c  # Parallel QuickSort — tasks without cutoff (high overhead)
│
├── first_bash.sh                       # SLURM script for OpenMP sum (64 threads)
├── first_bash_Serial.sh                # SLURM script for serial sum
├── Threads.sh                          # SLURM script for POSIX threads sum (10 cores)
│
├── ── MPI ──
├── MPI_TASK_A_FOX_Algorithm_Subham_Gaurav/
│   ├── FOX_Algorithm.c                 # Fox's algorithm — scalar elements, p = n²
│   └── TaskA.pdf                       # Task A specification
│
├── MPI_TASK_B_FOX_Algorithm_Subham_Gaurav/
│   ├── Fox_Algorithm_Task_B.c          # Fox's algorithm — block decomposition, p arbitrary perfect square
│   ├── MPI_bash.sh                     # SLURM script for MPI job (400 tasks)
│   └── TaskB.pdf                       # Task B specification
│
└── GroupI_Parallel_Programming_Presentation.pptx   # Group presentation slides
```

---

## Task 1 — Parallel Sum (OpenMP & POSIX Threads)

### Problem
Compute the sum of the first **100,000,000** natural numbers using various parallelisation strategies and compare performance.

### Implementations

| File | Strategy | Key OpenMP Directive |
|---|---|---|
| `Sum_Serial.c` | Single-threaded loop | — |
| `Sum_Threads.c` | 10 POSIX threads, partial sums | `pthread_create` / `pthread_join` |
| `sumwithopenmp.c` | OpenMP parallel for + reduction | `#pragma omp for reduction(+:total_sum)` |
| `sumwithopenmp_Sections.c` | 2 OpenMP sections + critical | `#pragma omp parallel sections` + `critical` |
| `sumwithopenmp_tasks.c` | 2 OpenMP tasks + taskwait | `#pragma omp task` + `taskwait` |

### Performance Results (64 threads, cluster)

| Implementation | Time (s) |
|---|---|
| Serial | ~0.521 |
| POSIX Threads (10 threads) | — (output: 5,000,000,050,000,000 ✓) |
| OpenMP Reduction | **~0.023** ✅ Fastest |
| OpenMP Sections | ~0.270 |
| OpenMP Tasks | ~1.836 |

> **Key Finding:** `#pragma omp for reduction` is the clear winner, as the compiler can auto-parallelize the loop with minimal synchronisation overhead. OpenMP tasks introduce significant scheduling overhead for a simple flat loop.

---

## Task 2 — Parallel QuickSort (OpenMP)

### Problem
Sort an integer array using QuickSort and evaluate the impact of different OpenMP parallelisation strategies.

### Implementations

| File | Strategy | Cutoff Threshold |
|---|---|---|
| `QuickSort_Serial.c` | Serial recursive QuickSort | — |
| `QuickSort_Parallel_With_Overhead.c` | OpenMP tasks — **no cutoff** | None (always creates tasks) |
| `QuickSort_Parallel.c` | OpenMP tasks — **with cutoff** (subarray > 1000) | 1,000 elements |
| `QuickSort_Parallel_Sections.c` | OpenMP **sections** (one level only) | — |

### Key Technique — Task Cutoff (`QuickSort_Parallel.c`)

```c
#pragma omp task shared(arr) if (p - 1 - start > 1000)
    quickSort(arr, start, p - 1);

#pragma omp task shared(arr) if (end - (p + 1) > 1000)
    quickSort(arr, p + 1, end);
```

Tasks are only spawned when the sub-array is large enough, avoiding the massive overhead of creating millions of micro-tasks.

### Performance Results (N = 1,000,000, 64 threads)

| Implementation | Time (s) |
|---|---|
| Serial | 0.564 |
| Parallel — With Overhead (no cutoff) | **55.21** ❌ Much worse |
| Parallel — No Overhead (cutoff > 1000) | **0.078** ✅ Best |
| Sections | 0.244 |

### Thread Scaling (Parallel w/ Cutoff, N = 1,000,000)

| Threads | Time (s) |
|---|---|
| 2 | 0.508 |
| 4 | 0.188 |
| 8 | 0.140 |
| 16 | 0.073 |
| 32 | 0.068 |
| 64 | 0.077 |

> **Key Finding:** Speedup plateaus around 32 threads due to diminishing returns and synchronisation overhead. The task cutoff is **critical** — without it, the runtime explodes by ~100× due to creating millions of tiny tasks.

---

## Task 3 — Fox's Algorithm for Parallel Matrix Multiplication (MPI)

Fox's Algorithm distributes an *n × n* matrix multiplication across a **√p × √p** process grid. Each step broadcasts a block of **A** along its row, multiplies it with the local block of **B**, then cyclically shifts **B** upward.

### Task A — Scalar Element Distribution (`FOX_Algorithm.c`)

- **Matrix size:** 5 × 5 (configurable via `#define n`)
- **Process requirement:** Exactly `n²` processes (one element per process)
- Each MPI process holds **one scalar element** of A and B
- Broadcast along row communicator, then circular upward shift of B

```
Constraint: mpirun -np 25 ./FOX_Algorithm   (for n=5)
```

### Task B — Block Decomposition (`Fox_Algorithm_Task_B.c`)

- **Matrix size:** 800 × 800 (configurable via global `N`)
- **Process requirement:** Any perfect square `p`; `N` must be divisible by `√p`
- Uses `MPI_Cart_create` for a 2D Cartesian process topology
- Each process owns a **block** of size `(N/√p) × (N/√p)` of both A and B
- Blocks of A are broadcast via `MPI_Bcast` along row communicators
- Blocks of B are cyclically shifted using `MPI_Sendrecv_replace`

```bash
# Example: 400 processes for an 800×800 matrix (20×20 process grid)
mpicc -o fox Fox_Algorithm_Task_B.c -lm
mpirun -np 400 ./fox
```

### Fox Algorithm Steps

```
For step = 0 to √p - 1:
  1. Root of broadcast = (my_row + step) % √p
  2. If my_col == root: copy localA → tempA
  3. Broadcast tempA along row communicator
  4. localC += tempA × localB  (local matrix multiply)
  5. Cyclically shift localB upward by one row
Gather all localC blocks to rank 0
```

---

## SLURM Batch Scripts

| Script | Purpose | Threads / Tasks |
|---|---|---|
| `first_bash_Serial.sh` | Run serial sum on cluster | 1 CPU |
| `first_bash.sh` | Run OpenMP sum (reduction) on cluster | 64 CPUs |
| `Threads.sh` | Run POSIX threads sum on cluster | 10 CPUs |
| `MPI_bash.sh` | Run MPI Fox Algorithm (Task B) | 400 MPI tasks |

### Example: Submitting a SLURM job

```bash
sbatch first_bash.sh
# Monitor: squeue -u <username>
# Output:  cat my_job_Sections_50.out
```

---

## Building & Running Locally

### Prerequisites
- GCC with OpenMP support (`gcc -fopenmp`)
- POSIX threads (`-lpthread`)
- MPI implementation (OpenMPI or MPICH)

### OpenMP programs

```bash
# Serial sum
gcc -fopenmp -o Sum_Serial Sum_Serial.c && ./Sum_Serial

# OpenMP reduction sum
gcc -fopenmp -o sumwithopenmp sumwithopenmp.c && ./sumwithopenmp

# Parallel QuickSort (with cutoff)
gcc -fopenmp -o QuickSort_Parallel QuickSort_Parallel.c && ./QuickSort_Parallel
```

### POSIX threads

```bash
gcc -o Sum_Threads Sum_Threads.c -lpthread && ./Sum_Threads
```

### MPI — Fox Algorithm Task A (n=5, 25 processes)

```bash
mpicc -o fox_a MPI_TASK_A_FOX_Algorithm_Subham_Gaurav/FOX_Algorithm.c -lm
mpirun -np 25 ./fox_a
```

### MPI — Fox Algorithm Task B (N=800, 400 processes)

```bash
mpicc -o fox_b MPI_TASK_B_FOX_Algorithm_Subham_Gaurav/Fox_Algorithm_Task_B.c -lm
mpirun -np 400 ./fox_b
```

---

## References

- Åbo Akademi University. *Matrix Multiplication and Fox's Algorithm.* [Semantic Scholar PDF](https://pdfs.semanticscholar.org/3d39/d9e575cdb1616ec082052e2be6bb6dd7d4f1.pdf)
- Thomas, M. (2017). *MPI Matrix Multiplication — Fox Algorithm.* SDSU CS 605. [Lecture Slides](https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPI-MatMatMult.pdf)
- Pacheco, P. *Parallel Programming with MPI.* Morgan-Kaufmann, 1997. Section 5.3.
- University of Nizhni Novgorod. *CS338 Lab02 — Introduction to Parallel Programming.* [Lab PDF](http://www.hpcc.unn.ru/mskurs/LAB/ENG/DOC/Lab02.pdf)
- GeeksForGeeks. [QuickSort using MPI, OMP and POSIX Threads](https://www.geeksforgeeks.org/implementation-of-quick-sort-using-mpi-omp-and-posix-thread/)
- AI5085 Course material — `row_wise_matrix_mult.c`
