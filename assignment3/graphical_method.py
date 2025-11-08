#!/usr/bin/env python3
"""
Graphical Method Solver for 2-Variable Linear Programming Problems

Solves both maximization and minimization problems with 2 variables
by plotting constraints, identifying feasible region, and evaluating
vertices.
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
from typing import List, Tuple, Dict, Optional
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend


class GraphicalMethodSolver:
    """Solver for 2-variable LP problems using graphical method."""
    
    def __init__(self, problem_name: str = "Problem"):
        """
        Initialize the graphical method solver.
        
        Args:
            problem_name: Name of the problem for display
        """
        self.problem_name = problem_name
        self.constraints = []
        self.objective = None
        self.is_maximization = True
        self.x_bounds = [0, 50]  # Default x-axis bounds
        self.y_bounds = [0, 50]  # Default y-axis bounds
        
    def add_constraint(self, a: float, b: float, c: float, sense: str = '<='):
        """
        Add a constraint: a*x + b*y <= c (or >=, =).
        
        Args:
            a: Coefficient of x
            b: Coefficient of y
            c: Right-hand side constant
            sense: Constraint sense: '<=', '>=', or '='
        """
        self.constraints.append({
            'a': a,
            'b': b,
            'c': c,
            'sense': sense
        })
    
    def set_objective(self, a: float, b: float, is_maximization: bool = True):
        """
        Set objective function: maximize/minimize a*x + b*y.
        
        Args:
            a: Coefficient of x
            b: Coefficient of y
            is_maximization: True for maximize, False for minimize
        """
        self.objective = {'a': a, 'b': b}
        self.is_maximization = is_maximization
    
    def set_bounds(self, x_min: float, x_max: float, y_min: float, y_max: float):
        """
        Set plotting bounds.
        
        Args:
            x_min: Minimum x value
            x_max: Maximum x value
            y_min: Minimum y value
            y_max: Maximum y value
        """
        self.x_bounds = [x_min, x_max]
        self.y_bounds = [y_min, y_max]
    
    def _find_intersections(self) -> List[Tuple[float, float]]:
        """
        Find all intersection points of constraint lines.
        
        Returns:
            List of (x, y) intersection points
        """
        intersections = []
        n = len(self.constraints)
        
        for i in range(n):
            for j in range(i + 1, n):
                # Solve system: a1*x + b1*y = c1, a2*x + b2*y = c2
                a1, b1, c1 = self.constraints[i]['a'], self.constraints[i]['b'], self.constraints[i]['c']
                a2, b2, c2 = self.constraints[j]['a'], self.constraints[j]['b'], self.constraints[j]['c']
                
                # Matrix form: [[a1, b1], [a2, b2]] * [x, y] = [c1, c2]
                A = np.array([[a1, b1], [a2, b2]])
                b_vec = np.array([c1, c2])
                
                try:
                    # Check if matrix is invertible
                    det = a1 * b2 - a2 * b1
                    if abs(det) > 1e-10:
                        solution = np.linalg.solve(A, b_vec)
                        x, y = solution[0], solution[1]
                        intersections.append((x, y))
                except:
                    pass  # Skip if no unique solution
        
        return intersections
    
    def _is_feasible(self, x: float, y: float) -> bool:
        """
        Check if a point (x, y) satisfies all constraints.
        
        Args:
            x: x coordinate
            y: y coordinate
            
        Returns:
            True if point is feasible
        """
        for constraint in self.constraints:
            a, b, c = constraint['a'], constraint['b'], constraint['c']
            sense = constraint['sense']
            value = a * x + b * y
            
            if sense == '<=':
                if value > c + 1e-10:  # Small tolerance for floating point
                    return False
            elif sense == '>=':
                if value < c - 1e-10:
                    return False
            elif sense == '=':
                if abs(value - c) > 1e-10:
                    return False
        
        # Check non-negativity (x, y >= 0)
        if x < -1e-10 or y < -1e-10:
            return False
        
        return True
    
    def _evaluate_objective(self, x: float, y: float) -> float:
        """
        Evaluate objective function at point (x, y).
        
        Args:
            x: x coordinate
            y: y coordinate
            
        Returns:
            Objective value
        """
        if self.objective is None:
            return 0
        return self.objective['a'] * x + self.objective['b'] * y
    
    def solve(self) -> Dict:
        """
        Solve the LP problem using graphical method.
        
        Returns:
            Dictionary with solution information
        """
        # Find all intersection points
        intersections = self._find_intersections()
        
        # Also check boundary points (intersections with axes)
        boundary_points = []
        
        # Intersections with x-axis (y = 0)
        for constraint in self.constraints:
            a, b, c = constraint['a'], constraint['b'], constraint['c']
            if abs(a) > 1e-10:
                x = c / a
                boundary_points.append((x, 0.0))
        
        # Intersections with y-axis (x = 0)
        for constraint in self.constraints:
            a, b, c = constraint['a'], constraint['b'], constraint['c']
            if abs(b) > 1e-10:
                y = c / b
                boundary_points.append((0.0, y))
        
        # Combine all candidate points
        all_points = intersections + boundary_points
        
        # Filter feasible points
        feasible_points = []
        for point in all_points:
            x, y = point
            if self._is_feasible(x, y):
                feasible_points.append(point)
        
        if not feasible_points:
            return {
                'status': 'infeasible',
                'solution': None,
                'objective_value': None,
                'feasible_points': []
            }
        
        # Evaluate objective at all feasible points
        objective_values = [self._evaluate_objective(x, y) for x, y in feasible_points]
        
        if self.is_maximization:
            optimal_idx = np.argmax(objective_values)
        else:
            optimal_idx = np.argmin(objective_values)
        
        optimal_point = feasible_points[optimal_idx]
        optimal_value = objective_values[optimal_idx]
        
        return {
            'status': 'optimal',
            'solution': optimal_point,
            'objective_value': optimal_value,
            'feasible_points': feasible_points,
            'all_candidate_points': all_points
        }
    
    def plot(self, output_file: Optional[str] = None) -> plt.Figure:
        """
        Plot the constraints, feasible region, and optimal solution.
        
        Args:
            output_file: Optional filename to save the plot
            
        Returns:
            Matplotlib figure
        """
        fig, ax = plt.subplots(figsize=(10, 8))
        
        x_min, x_max = self.x_bounds
        y_min, y_max = self.y_bounds
        
        # Plot constraint lines
        x_plot = np.linspace(x_min, x_max, 1000)
        
        for constraint in self.constraints:
            a, b, c = constraint['a'], constraint['b'], constraint['c']
            sense = constraint['sense']
            
            if abs(b) > 1e-10:
                # Can solve for y: y = (c - a*x) / b
                y_plot = (c - a * x_plot) / b
                ax.plot(x_plot, y_plot, 'b-', linewidth=2, label=f'{a:.1f}x + {b:.1f}y {sense} {c:.1f}')
            else:
                # Vertical line: x = c / a
                x_val = c / a
                ax.axvline(x=x_val, color='b', linewidth=2, label=f'x {sense} {x_val:.1f}')
        
        # Shade feasible region
        # Create a grid and check feasibility
        xx, yy = np.meshgrid(np.linspace(x_min, x_max, 100), 
                            np.linspace(y_min, y_max, 100))
        feasible_mask = np.zeros_like(xx, dtype=bool)
        
        for i in range(xx.shape[0]):
            for j in range(xx.shape[1]):
                feasible_mask[i, j] = self._is_feasible(xx[i, j], yy[i, j])
        
        ax.contourf(xx, yy, feasible_mask.astype(float), levels=[0.5, 1.5], 
                   colors=['lightgreen'], alpha=0.3)
        
        # Solve and plot optimal solution
        result = self.solve()
        
        if result['status'] == 'optimal':
            x_opt, y_opt = result['solution']
            ax.plot(x_opt, y_opt, 'ro', markersize=10, label='Optimal Solution')
            ax.text(x_opt + 0.5, y_opt + 0.5, f'({x_opt:.2f}, {y_opt:.2f})', 
                   fontsize=10, bbox=dict(boxstyle='round', facecolor='yellow', alpha=0.7))
        
        # Plot feasible vertices
        for point in result.get('feasible_points', []):
            x, y = point
            ax.plot(x, y, 'go', markersize=6, alpha=0.5)
        
        ax.set_xlim(x_min, x_max)
        ax.set_ylim(y_min, y_max)
        ax.set_xlabel('x', fontsize=12)
        ax.set_ylabel('y', fontsize=12)
        ax.set_title(f'{self.problem_name}\nObjective: {"Maximize" if self.is_maximization else "Minimize"} '
                    f'{self.objective["a"]:.1f}x + {self.objective["b"]:.1f}y' if self.objective else '',
                    fontsize=14)
        ax.grid(True, alpha=0.3)
        ax.legend(loc='upper right', fontsize=8)
        ax.axhline(y=0, color='k', linewidth=0.5)
        ax.axvline(x=0, color='k', linewidth=0.5)
        
        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            print(f"Plot saved to {output_file}")
        
        return fig


def solve_scenario_1():
    """Solve Scenario 1: Server Maintenance Tasks"""
    solver = GraphicalMethodSolver("Scenario 1: Server Maintenance")
    solver.set_bounds(0, 50, 0, 50)
    solver.set_objective(40, 30, is_maximization=True)
    solver.add_constraint(3, 2, 120, '<=')  # Technician hours
    solver.add_constraint(2, 1, 70, '<=')   # Risk budget
    solver.add_constraint(0, 1, 10, '>=')   # Minimum type B tasks
    
    result = solver.solve()
    print("\nScenario 1 Results:")
    print(f"Status: {result['status']}")
    if result['status'] == 'optimal':
        print(f"Optimal solution: x = {result['solution'][0]:.2f}, y = {result['solution'][1]:.2f}")
        print(f"Optimal value: Z* = {result['objective_value']:.2f}")
    
    solver.plot('assignment3/scenario1_plot.png')
    return result


def solve_scenario_2():
    """Solve Scenario 2: Software Products"""
    solver = GraphicalMethodSolver("Scenario 2: Software Products")
    solver.set_bounds(0, 15, 0, 10)
    solver.set_objective(4, 6, is_maximization=True)
    solver.add_constraint(3, 6, 36, '<=')  # Developer hours
    solver.add_constraint(0, 1, 8, '<=')   # Maximum desktop tools
    
    result = solver.solve()
    print("\nScenario 2 Results:")
    print(f"Status: {result['status']}")
    if result['status'] == 'optimal':
        print(f"Optimal solution: x1 = {result['solution'][0]:.2f}, x2 = {result['solution'][1]:.2f}")
        print(f"Optimal value: Z* = {result['objective_value']:.2f}")
    
    solver.plot('assignment3/scenario2_plot.png')
    return result


def solve_scenario_3():
    """Solve Scenario 3: Data Center Resource Allocation"""
    solver = GraphicalMethodSolver("Scenario 3: Data Center Resources")
    solver.set_bounds(0, 10, 0, 10)
    solver.set_objective(5, 4, is_maximization=True)
    solver.add_constraint(1, -1, 2, '>=')  # x1 >= x2 + 2
    solver.add_constraint(1, 1, 6, '>=')   # x1 + x2 >= 6
    
    result = solver.solve()
    print("\nScenario 3 Results:")
    print(f"Status: {result['status']}")
    if result['status'] == 'optimal':
        print(f"Optimal solution: x1 = {result['solution'][0]:.2f}, x2 = {result['solution'][1]:.2f}")
        print(f"Optimal value: Z* = {result['objective_value']:.2f}")
    else:
        print("Note: This problem may be unbounded (feasible region extends to infinity)")
    
    solver.plot('assignment3/scenario3_plot.png')
    return result


def solve_scenario_4():
    """Solve Scenario 4: Cafeteria Meal Plans"""
    solver = GraphicalMethodSolver("Scenario 4: Cafeteria Meal Plans")
    solver.set_bounds(0, 10, 0, 15)
    solver.set_objective(1000, 1500, is_maximization=False)  # Minimize cost
    solver.add_constraint(2000, 1000, 14000, '>=')  # kcal requirement
    solver.add_constraint(30, 60, 480, '>=')        # protein requirement
    
    result = solver.solve()
    print("\nScenario 4 Results:")
    print(f"Status: {result['status']}")
    if result['status'] == 'optimal':
        print(f"Optimal solution: x1 = {result['solution'][0]:.2f}, x2 = {result['solution'][1]:.2f}")
        print(f"Optimal value: Z* = {result['objective_value']:.2f} tenge")
    
    solver.plot('assignment3/scenario4_plot.png')
    return result


def solve_scenario_5():
    """Solve Scenario 5: Weekly Budget"""
    solver = GraphicalMethodSolver("Scenario 5: Weekly Budget")
    solver.set_bounds(0, 5000, 0, 15000)
    solver.set_objective(6, 1, is_maximization=False)  # Minimize
    solver.add_constraint(1, 1, 12000, '<=')  # Weekly allowance
    solver.add_constraint(1, 0, 1200, '>=')   # Minimum meal cost
    solver.add_constraint(-2, 1, 0, '>=')     # x2 >= 2*x1, so -2*x1 + x2 >= 0
    
    result = solver.solve()
    print("\nScenario 5 Results:")
    print(f"Status: {result['status']}")
    if result['status'] == 'optimal':
        print(f"Optimal solution: x1 = {result['solution'][0]:.2f}, x2 = {result['solution'][1]:.2f}")
        print(f"Optimal value: Z* = {result['objective_value']:.2f}")
    
    solver.plot('assignment3/scenario5_plot.png')
    return result


def solve_scenario_6():
    """Solve Scenario 6: Weekly Budget (Infeasible)"""
    solver = GraphicalMethodSolver("Scenario 6: Weekly Budget (Infeasible)")
    solver.set_bounds(0, 5000, 0, 15000)
    solver.set_objective(1, 1, is_maximization=False)  # Minimize
    solver.add_constraint(1, 1, 5000, '<=')   # Weekly allowance
    solver.add_constraint(1, 0, 4000, '>=')   # Minimum meal cost
    solver.add_constraint(-2, 1, 0, '>=')     # x2 >= 2*x1, so -2*x1 + x2 >= 0
    
    result = solver.solve()
    print("\nScenario 6 Results:")
    print(f"Status: {result['status']}")
    if result['status'] == 'infeasible':
        print("Problem is INFEASIBLE: No solution satisfies all constraints")
        print("Reason: x1 >= 4000 and x2 >= 2*x1 requires x1 + x2 >= 12000,")
        print("        but x1 + x2 <= 5000, which is impossible.")
    
    solver.plot('assignment3/scenario6_plot.png')
    return result


def main():
    """Solve all scenarios."""
    print("=" * 60)
    print("Graphical Method - Solving All Scenarios")
    print("=" * 60)
    
    solve_scenario_1()
    solve_scenario_2()
    solve_scenario_3()
    solve_scenario_4()
    solve_scenario_5()
    solve_scenario_6()
    
    print("\n" + "=" * 60)
    print("All scenarios completed. Plots saved to assignment3/ directory.")
    print("=" * 60)


if __name__ == '__main__':
    main()

