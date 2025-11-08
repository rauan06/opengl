# Assignment III: Graphical Method and Simplex

This assignment implements solutions for linear programming problems using both the graphical method (for 2-variable problems) and the simplex method (for higher-dimensional problems).

## Structure

```
assignment3/
├── README.md                    # This file
├── assignment3_solutions.md     # Detailed solutions for all problems
├── simplex_solver.py            # Simplex method implementation
├── graphical_method.py          # Graphical method solver and visualizations
├── test_problem1.json          # Test problem 1 (3 variables)
├── test_problem2.json          # Test problem 2 (2 variables)
├── test_problem3.csv           # Test problem 3 (CSV format)
└── scenario*.png               # Generated plots for each scenario
```

## Requirements

- Python 3.6+
- NumPy
- Matplotlib

Install dependencies:
```bash
pip install numpy matplotlib
```

## Task A: Graphical Method

The graphical method solver solves 2-variable LP problems by:
1. Plotting all constraint lines
2. Identifying the feasible region
3. Evaluating the objective function at each vertex
4. Finding the optimal solution

### Running Graphical Method Scenarios

```bash
python graphical_method.py
```

This will:
- Solve all 6 scenarios
- Generate plots for each scenario
- Print optimal solutions to the console

### Individual Scenarios

**Scenario 1:** Server Maintenance Tasks (Maximization)
- Objective: Maximize z = 40x + 30y
- Constraints: Technician hours, risk budget, minimum type B tasks

**Scenario 2:** Software Products (Maximization)
- Objective: Maximize z = 4x₁ + 6x₂
- Constraints: Developer hours, maximum desktop tools

**Scenario 3:** Data Center Resource Allocation (Maximization)
- Objective: Maximize z = 5x₁ + 4x₂
- Constraints: CPU/storage relationship, minimum resources

**Scenario 4:** Cafeteria Meal Plans (Minimization)
- Objective: Minimize z = 1000x₁ + 1500x₂
- Constraints: Calorie and protein requirements

**Scenario 5:** Weekly Budget (Minimization)
- Objective: Minimize z = 6x₁ + x₂
- Constraints: Allowance, minimum meals, entertainment requirement

**Scenario 6:** Weekly Budget - Infeasible (Minimization)
- Objective: Minimize z = x₁ + x₂
- Constraints: Conflicting requirements (INFEASIBLE)

## Task B: Simplex Method (By Hand)

See `assignment3_solutions.md` for detailed step-by-step solution of the 3-variable problem:
- Maximize Z = 6S + 8M + 10L
- Subject to constraints with M ≥ 10

The solution includes:
1. Conversion to standard form
2. Initial tableau
3. Multiple pivot iterations
4. Final optimal solution

## Task BB: Simplex Solver Implementation

The simplex solver implements the standard simplex method for maximization problems of the form:
```
maximize: c^T x
subject to: Ax ≤ b, x ≥ 0
```

### Usage

#### JSON Input Format

Create a JSON file with the following structure:
```json
{
  "c": [c1, c2, ..., cn],
  "A": [[a11, a12, ...], [a21, a22, ...], ...],
  "b": [b1, b2, ..., bm]
}
```

Example:
```bash
python simplex_solver.py test_problem1.json
```

#### CSV Input Format

CSV format:
- First row: objective coefficients c
- Next m rows: constraint matrix A (one row per constraint)
- Last row: right-hand side vector b

Example:
```bash
python simplex_solver.py test_problem3.csv
```

#### Save Results

To save results to a file:
```bash
python simplex_solver.py test_problem1.json output.json
```

### Output

The solver outputs:
- **Status**: `optimal`, `unbounded`, or `max_iterations`
- **Solution**: Optimal solution vector x*
- **Objective Value**: Optimal objective value Z*
- **Iterations**: Number of simplex iterations performed

Example output:
```
Problem loaded:
  Objective: maximize [6 8 10]
  Constraints: 3 constraints, 3 variables

Solution:
  Status: optimal
  Optimal solution: x = [30.0, 20.0, 0.0]
  Optimal objective value: Z* = 340.0000
  Iterations: 3
```

## Testing

### Test Problem 1 (3 variables)
```bash
python simplex_solver.py test_problem1.json
```

### Test Problem 2 (2 variables)
```bash
python simplex_solver.py test_problem2.json
```

### Test Problem 3 (CSV format)
```bash
python simplex_solver.py test_problem3.csv
```

## Implementation Details

### Simplex Solver

The simplex solver (`simplex_solver.py`) implements:
- **Initialization**: Converts problem to standard form with slack variables
- **Pivoting**: Standard pivot operations
- **Entering Variable**: Selects variable with most negative coefficient in objective row
- **Leaving Variable**: Uses minimum ratio test
- **Optimality Check**: Checks if all objective coefficients are non-negative
- **Unbounded Detection**: Checks if no leaving variable can be found

### Graphical Method Solver

The graphical method solver (`graphical_method.py`) implements:
- **Constraint Plotting**: Plots all constraint lines
- **Feasible Region**: Identifies and shades feasible region
- **Vertex Finding**: Finds all intersection points of constraints
- **Optimal Solution**: Evaluates objective at all feasible vertices
- **Visualization**: Generates plots with constraints, feasible region, and optimal solution

## Notes

- The simplex solver handles maximization problems only (minimization can be converted by negating the objective)
- All problems assume non-negativity constraints (x ≥ 0)
- The graphical method works for 2-variable problems only
- Floating-point tolerances are used for equality comparisons (1e-10)

## Solutions

Detailed solutions for all problems are provided in `assignment3_solutions.md`, including:
- Mathematical formulations
- Graphical method solutions with vertex evaluations
- Step-by-step simplex method iterations
- Final optimal solutions

