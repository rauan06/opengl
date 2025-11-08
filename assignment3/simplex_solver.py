#!/usr/bin/env python3
"""
Simplex Solver for Linear Programming Problems

Solves maximization problems of the form:
    maximize: c^T x
    subject to: Ax <= b, x >= 0

Input: JSON or CSV file containing c, A, b
Output: optimal solution x*, objective value Z*, and status
"""

import json
import csv
import numpy as np
import sys
from typing import Tuple, Optional, Dict, List


class SimplexSolver:
    """Simplex method solver for linear programming problems."""
    
    def __init__(self, c: np.ndarray, A: np.ndarray, b: np.ndarray):
        """
        Initialize the simplex solver.
        
        Args:
            c: Objective function coefficients (1D array)
            A: Constraint matrix (2D array)
            b: Right-hand side vector (1D array)
        """
        self.c = np.array(c, dtype=float)
        self.A = np.array(A, dtype=float)
        self.b = np.array(b, dtype=float)
        
        # Validate inputs
        if len(self.c.shape) != 1:
            raise ValueError("c must be a 1D array")
        if len(self.A.shape) != 2:
            raise ValueError("A must be a 2D array")
        if len(self.b.shape) != 1:
            raise ValueError("b must be a 1D array")
        if self.A.shape[0] != len(self.b):
            raise ValueError("Number of rows in A must equal length of b")
        if self.A.shape[1] != len(self.c):
            raise ValueError("Number of columns in A must equal length of c")
        
        self.n = len(self.c)  # number of variables
        self.m = len(self.b)  # number of constraints
        
        # Tableau: [A | I | b] for constraints, [-c | 0 | 0] for objective
        self.tableau = None
        self.basic_vars = None  # indices of basic variables
        self.iteration = 0
        self.max_iterations = 1000
        
    def _initialize_tableau(self):
        """Initialize the simplex tableau."""
        # Create identity matrix for slack variables
        I = np.eye(self.m)
        
        # Combine A, I, and b
        constraints = np.hstack([self.A, I, self.b.reshape(-1, 1)])
        
        # Objective row: -c (for maximization), zeros for slacks, 0 for RHS
        obj_row = np.hstack([-self.c, np.zeros(self.m), np.array([0.0])])
        
        # Complete tableau
        self.tableau = np.vstack([constraints, obj_row])
        
        # Basic variables are the slack variables (indices n to n+m-1)
        self.basic_vars = list(range(self.n, self.n + self.m))
        
    def _find_entering_variable(self) -> Optional[int]:
        """
        Find the entering variable (variable with most negative coefficient in objective row).
        
        Returns:
            Index of entering variable, or None if optimal
        """
        obj_row = self.tableau[-1, :-1]
        entering_idx = None
        min_coeff = 0
        
        # Check only decision variables (first n columns)
        for i in range(self.n):
            if obj_row[i] < min_coeff - 1e-10:  # Small tolerance for floating point
                min_coeff = obj_row[i]
                entering_idx = i
        
        return entering_idx
    
    def _find_leaving_variable(self, entering_idx: int) -> Optional[int]:
        """
        Find the leaving variable using minimum ratio test.
        
        Args:
            entering_idx: Index of entering variable
            
        Returns:
            Index of basic variable to leave, or None if unbounded
        """
        ratios = []
        entering_col = self.tableau[:-1, entering_idx]
        rhs_col = self.tableau[:-1, -1]
        
        for i in range(self.m):
            if entering_col[i] > 1e-10:  # Avoid division by zero
                ratio = rhs_col[i] / entering_col[i]
                if ratio >= 0:  # Only consider non-negative ratios
                    ratios.append((ratio, i))
        
        if not ratios:
            return None  # Unbounded
        
        # Find minimum ratio
        min_ratio, leaving_row = min(ratios, key=lambda x: x[0])
        return leaving_row
    
    def _pivot(self, entering_idx: int, leaving_row: int):
        """
        Perform pivot operation.
        
        Args:
            entering_idx: Index of entering variable
            leaving_row: Row index of leaving basic variable
        """
        pivot_element = self.tableau[leaving_row, entering_idx]
        
        # Normalize pivot row
        self.tableau[leaving_row, :] /= pivot_element
        
        # Update other rows
        for i in range(self.m + 1):
            if i != leaving_row:
                multiplier = self.tableau[i, entering_idx]
                self.tableau[i, :] -= multiplier * self.tableau[leaving_row, :]
        
        # Update basic variables
        self.basic_vars[leaving_row] = entering_idx
    
    def solve(self) -> Dict:
        """
        Solve the linear programming problem using simplex method.
        
        Returns:
            Dictionary containing:
                - status: 'optimal', 'unbounded', or 'infeasible'
                - solution: optimal solution vector
                - objective_value: optimal objective value
                - iterations: number of iterations performed
        """
        self._initialize_tableau()
        
        while self.iteration < self.max_iterations:
            # Check for optimality
            entering_idx = self._find_entering_variable()
            if entering_idx is None:
                # All coefficients non-negative, optimal solution found
                return self._extract_solution('optimal')
            
            # Find leaving variable
            leaving_row = self._find_leaving_variable(entering_idx)
            if leaving_row is None:
                return self._extract_solution('unbounded')
            
            # Perform pivot
            self._pivot(entering_idx, leaving_row)
            self.iteration += 1
        
        return self._extract_solution('max_iterations')
    
    def _extract_solution(self, status: str) -> Dict:
        """
        Extract solution from current tableau.
        
        Args:
            status: Status of the solution
            
        Returns:
            Solution dictionary
        """
        solution = np.zeros(self.n)
        rhs_col = self.tableau[:-1, -1]
        obj_value = self.tableau[-1, -1]
        
        # Extract values of basic variables
        for i, var_idx in enumerate(self.basic_vars):
            if var_idx < self.n:  # Decision variable
                solution[var_idx] = max(0, rhs_col[i])  # Ensure non-negative
        
        return {
            'status': status,
            'solution': solution.tolist(),
            'objective_value': float(obj_value),
            'iterations': self.iteration
        }


def load_from_json(filename: str) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Load problem data from JSON file.
    
    Expected format:
    {
        "c": [c1, c2, ..., cn],
        "A": [[a11, a12, ...], [a21, a22, ...], ...],
        "b": [b1, b2, ..., bm]
    }
    
    Args:
        filename: Path to JSON file
        
    Returns:
        Tuple of (c, A, b)
    """
    with open(filename, 'r') as f:
        data = json.load(f)
    
    c = np.array(data['c'])
    A = np.array(data['A'])
    b = np.array(data['b'])
    
    return c, A, b


def load_from_csv(filename: str) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Load problem data from CSV file.
    
    Expected format:
    - First row: objective coefficients c
    - Next m rows: constraint matrix A
    - Last row: right-hand side vector b
    
    Args:
        filename: Path to CSV file
        
    Returns:
        Tuple of (c, A, b)
    """
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        rows = list(reader)
    
    if len(rows) < 3:
        raise ValueError("CSV file must have at least 3 rows (c, A rows, b)")
    
    # First row: objective coefficients
    c = np.array([float(x) for x in rows[0]])
    n = len(c)
    
    # Last row: right-hand side
    b = np.array([float(x) for x in rows[-1]])
    m = len(b)
    
    # Middle rows: constraint matrix
    if len(rows) != m + 2:
        raise ValueError(f"Expected {m + 2} rows, got {len(rows)}")
    
    A = np.array([[float(x) for x in rows[i+1]] for i in range(m)])
    
    if A.shape[1] != n:
        raise ValueError(f"Constraint matrix columns ({A.shape[1]}) must match c length ({n})")
    
    return c, A, b


def save_solution(filename: str, result: Dict):
    """
    Save solution to JSON file.
    
    Args:
        filename: Output filename
        result: Solution dictionary
    """
    with open(filename, 'w') as f:
        json.dump(result, f, indent=2)


def main():
    """Main function to run the simplex solver."""
    if len(sys.argv) < 2:
        print("Usage: python simplex_solver.py <input_file> [output_file]")
        print("Input file can be JSON or CSV format")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    try:
        # Load problem data
        if input_file.endswith('.json'):
            c, A, b = load_from_json(input_file)
        elif input_file.endswith('.csv'):
            c, A, b = load_from_csv(input_file)
        else:
            # Try JSON first, then CSV
            try:
                c, A, b = load_from_json(input_file)
            except:
                c, A, b = load_from_csv(input_file)
        
        print("Problem loaded:")
        print(f"  Objective: maximize {c}")
        print(f"  Constraints: {A.shape[0]} constraints, {A.shape[1]} variables")
        print()
        
        # Solve
        solver = SimplexSolver(c, A, b)
        result = solver.solve()
        
        # Print results
        print("Solution:")
        print(f"  Status: {result['status']}")
        if result['status'] == 'optimal':
            print(f"  Optimal solution: x = {result['solution']}")
            print(f"  Optimal objective value: Z* = {result['objective_value']:.4f}")
        print(f"  Iterations: {result['iterations']}")
        
        # Save if output file specified
        if output_file:
            save_solution(output_file, result)
            print(f"\nSolution saved to {output_file}")
        
        return result
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()

