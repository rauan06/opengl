# Assignment III: Graphical Method and Simplex

## Task A: Graphical Method

### Scenario 1: Server Maintenance Tasks

**Problem Formulation:**
- Let \(x\) = number of type A tasks, \(y\) = number of type B tasks
- Maximize: \(z = 40x + 30y\)
- Subject to:
  - \(3x + 2y \leq 120\) (technician hours)
  - \(2x + y \leq 70\) (risk budget)
  - \(y \geq 10\) (minimum type B tasks)
  - \(x, y \geq 0\)

**Solution:**
Plotting constraints:
1. \(3x + 2y = 120\) → \(y = 60 - 1.5x\)
2. \(2x + y = 70\) → \(y = 70 - 2x\)
3. \(y = 10\) (horizontal line)

**Feasible Region Vertices:**
- Intersection of \(y = 10\) and \(3x + 2y = 120\): \(x = 33.33, y = 10\)
- Intersection of \(3x + 2y = 120\) and \(2x + y = 70\): \(x = 20, y = 30\)
- Intersection of \(y = 10\) and \(2x + y = 70\): \(x = 30, y = 10\)

**Optimal Solution:**
Evaluating at vertices:
- \((0, 10)\): \(z = 300\)
- \((30, 10)\): \(z = 1500\)
- \((33.33, 10)\): \(z = 1633.33\)
- \((20, 30)\): \(z = 1700\) ✓ **OPTIMAL**

**Answer:** \(x^* = 20, y^* = 30, z^* = 1700\)

---

### Scenario 2: Software Products

**Problem Formulation:**
- Let \(x_1\) = number of mobile apps, \(x_2\) = number of desktop tools
- Maximize: \(z = 4x_1 + 6x_2\)
- Subject to:
  - \(3x_1 + 6x_2 \leq 36\) (developer hours)
  - \(x_2 \leq 8\) (maximum desktop tools)
  - \(x_1, x_2 \geq 0\)

**Solution:**
Plotting constraints:
1. \(3x_1 + 6x_2 = 36\) → \(x_2 = 6 - 0.5x_1\)
2. \(x_2 = 8\) (horizontal line)

**Feasible Region Vertices:**
- \((0, 0)\): \(z = 0\)
- \((12, 0)\): \(z = 48\)
- \((0, 6)\): \(z = 36\)
- \((0, 8)\): Not feasible (violates first constraint)

**Optimal Solution:**
- \((12, 0)\): \(z = 48\) ✓ **OPTIMAL**

**Answer:** \(x_1^* = 12, x_2^* = 0, z^* = 48\)

---

### Scenario 3: Data Center Resource Allocation

**Problem Formulation:**
- Let \(x_1\) = CPU time units, \(x_2\) = storage units
- Maximize: \(z = 5x_1 + 4x_2\)
- Subject to:
  - \(x_1 \geq x_2 + 2\) → \(x_1 - x_2 \geq 2\)
  - \(x_1 + x_2 \geq 6\)
  - \(x_1, x_2 \geq 0\)

**Solution:**
Plotting constraints:
1. \(x_1 - x_2 = 2\) → \(x_1 = x_2 + 2\)
2. \(x_1 + x_2 = 6\) → \(x_2 = 6 - x_1\)

**Feasible Region Vertices:**
- Intersection: \(x_2 + 2 + x_2 = 6\) → \(x_2 = 2, x_1 = 4\)
- \((2, 0)\): \(z = 10\)
- \((4, 2)\): \(z = 28\) ✓ **OPTIMAL**
- \((6, 0)\): \(z = 30\) (but check if feasible: \(6 - 0 \geq 2\) ✓, but \(6 + 0 \geq 6\) ✓)

Actually, the feasible region is unbounded above. Let's check:
- At \((4, 2)\): \(z = 28\)
- At \((6, 0)\): \(z = 30\)
- As \(x_1 \to \infty\) along the line \(x_1 - x_2 = 2\), \(z \to \infty\)

Wait, we need to check if there's an upper bound. Since the problem is to maximize and the feasible region is unbounded, we need to check if there are additional constraints or if this is an unbounded problem.

For a bounded solution, let's assume we need to find the minimum point in the feasible region. Actually, re-reading: we're maximizing, and the feasible region extends to infinity along \(x_1\). However, typically in LP problems, we might be missing a constraint, or this could be unbounded.

Let me reconsider: If we're maximizing \(5x_1 + 4x_2\) subject to \(x_1 - x_2 \geq 2\) and \(x_1 + x_2 \geq 6\), the feasible region is unbounded to the right, so the maximum is unbounded. However, if we interpret this as finding the vertex with minimum cost (closest to origin), it would be \((4, 2)\).

**Assuming bounded region, Optimal Solution:**
- \((4, 2)\): \(z = 28\) (corner point of feasible region)

**Answer:** \(x_1^* = 4, x_2^* = 2, z^* = 28\) (if bounded) or unbounded if no upper constraints.

---

### Scenario 4: Cafeteria Meal Plans (Minimization)

**Problem Formulation:**
- Let \(x_1\) = number of basic meal boxes, \(x_2\) = number of protein meal boxes
- Minimize: \(z = 1000x_1 + 1500x_2\)
- Subject to:
  - \(2000x_1 + 1000x_2 \geq 14000\) (kcal requirement)
  - \(30x_1 + 60x_2 \geq 480\) (protein requirement)
  - \(x_1, x_2 \geq 0\)

**Solution:**
Plotting constraints:
1. \(2000x_1 + 1000x_2 = 14000\) → \(x_2 = 14 - 2x_1\)
2. \(30x_1 + 60x_2 = 480\) → \(x_2 = 8 - 0.5x_1\)

**Feasible Region Vertices:**
- Intersection: \(14 - 2x_1 = 8 - 0.5x_1\) → \(1.5x_1 = 6\) → \(x_1 = 4, x_2 = 6\)
- \((7, 0)\): \(z = 7000\)
- \((0, 14)\): \(z = 21000\)
- \((4, 6)\): \(z = 13000\) ✓ **OPTIMAL**

**Answer:** \(x_1^* = 4, x_2^* = 6, z^* = 13000\) tenge

---

### Scenario 5: Weekly Budget (Minimization)

**Problem Formulation:**
- Let \(x_1\) = amount spent on meals, \(x_2\) = amount spent on entertainment
- Minimize: \(z = 6x_1 + x_2\)
- Subject to:
  - \(x_1 + x_2 \leq 12000\) (weekly allowance)
  - \(x_1 \geq 1200\) (minimum meal cost)
  - \(x_2 \geq 2x_1\) (entertainment at least twice meals)
  - \(x_1, x_2 \geq 0\)

**Solution:**
Plotting constraints:
1. \(x_1 + x_2 = 12000\) → \(x_2 = 12000 - x_1\)
2. \(x_1 = 1200\) (vertical line)
3. \(x_2 = 2x_1\) (line through origin)

**Feasible Region Vertices:**
- Intersection of \(x_1 = 1200\) and \(x_2 = 2x_1\): \((1200, 2400)\)
- Intersection of \(x_1 = 1200\) and \(x_1 + x_2 = 12000\): \((1200, 10800)\)
- Intersection of \(x_2 = 2x_1\) and \(x_1 + x_2 = 12000\): \(x_1 + 2x_1 = 12000\) → \(x_1 = 4000, x_2 = 8000\)

Check feasibility:
- \((1200, 2400)\): \(1200 + 2400 = 3600 \leq 12000\) ✓, \(2400 \geq 2(1200) = 2400\) ✓
- \((4000, 8000)\): \(4000 + 8000 = 12000 \leq 12000\) ✓, \(8000 \geq 2(4000) = 8000\) ✓

**Optimal Solution:**
- \((1200, 2400)\): \(z = 6(1200) + 2400 = 9600\) ✓ **OPTIMAL**
- \((4000, 8000)\): \(z = 6(4000) + 8000 = 32000\)

**Answer:** \(x_1^* = 1200, x_2^* = 2400, z^* = 9600\)

---

### Scenario 6: Weekly Budget (Minimization)

**Problem Formulation:**
- Let \(x_1\) = amount spent on meals, \(x_2\) = amount spent on entertainment
- Minimize: \(z = x_1 + x_2\)
- Subject to:
  - \(x_1 + x_2 \leq 5000\) (weekly allowance)
  - \(x_1 \geq 4000\) (minimum meal cost)
  - \(x_2 \geq 2x_1\) (entertainment at least twice meals)
  - \(x_1, x_2 \geq 0\)

**Solution:**
Plotting constraints:
1. \(x_1 + x_2 = 5000\) → \(x_2 = 5000 - x_1\)
2. \(x_1 = 4000\) (vertical line)
3. \(x_2 = 2x_1\) (line through origin)

**Feasibility Check:**
- If \(x_1 = 4000\) and \(x_2 \geq 2(4000) = 8000\), then \(x_1 + x_2 \geq 12000\)
- But \(x_1 + x_2 \leq 5000\)
- **INFEASIBLE** - No solution exists that satisfies all constraints.

**Answer:** **INFEASIBLE PROBLEM**

---

## Task B: Simplex Method (3 Variables)

### Problem
Maximize \(Z = 6S + 8M + 10L\)

Subject to:
- \(2S + 3M + 5L \leq 150\)
- \(4S + 2M + 5L \leq 180\)
- \(3S + 2M + 4L \leq 160\)
- \(M \geq 10\)
- \(S, M, L \geq 0\)

### Step 1: Convert to Standard Form

Introduce slack variables \(s_1, s_2, s_3\) for the first three constraints and surplus variable \(s_4\) and artificial variable \(a_1\) for the \(M \geq 10\) constraint:

Maximize \(Z = 6S + 8M + 10L + 0s_1 + 0s_2 + 0s_3 + 0s_4 - Ma_1\)

Subject to:
- \(2S + 3M + 5L + s_1 = 150\)
- \(4S + 2M + 5L + s_2 = 180\)
- \(3S + 2M + 4L + s_3 = 160\)
- \(M - s_4 + a_1 = 10\)
- \(S, M, L, s_1, s_2, s_3, s_4, a_1 \geq 0\)

### Step 2: Initial Tableau

Using Big-M method, we'll work with the problem after eliminating the artificial variable (using two-phase or adjusting the objective).

**Initial Tableau (after setting up with artificial variable):**

```
BV | S  | M  | L  | s1 | s2 | s3 | s4 | a1 | RHS
---+----+----+----+----+----+----+----+----+----
s1 | 2  | 3  | 5  | 1  | 0  | 0  | 0  | 0  | 150
s2 | 4  | 2  | 5  | 0  | 1  | 0  | 0  | 0  | 180
s3 | 3  | 2  | 4  | 0  | 0  | 1  | 0  | 0  | 160
a1 | 0  | 1  | 0  | 0  | 0  | 0  | -1 | 1  | 10
---+----+----+----+----+----+----+----+----+----
-Z | -6 | -8 | -10| 0  | 0  | 0  | 0  | M  | 0
```

For Phase 1, we minimize \(a_1\), then proceed to Phase 2.

**Alternative approach:** Since \(M \geq 10\), we can substitute \(M = M' + 10\) where \(M' \geq 0\), then solve and add 10 back.

Let \(M = M' + 10\), \(M' \geq 0\):

Maximize \(Z = 6S + 8(M' + 10) + 10L = 6S + 8M' + 10L + 80\)

Subject to:
- \(2S + 3(M' + 10) + 5L \leq 150\) → \(2S + 3M' + 5L \leq 120\)
- \(4S + 2(M' + 10) + 5L \leq 180\) → \(4S + 2M' + 5L \leq 160\)
- \(3S + 2(M' + 10) + 4L \leq 160\) → \(3S + 2M' + 4L \leq 140\)
- \(S, M', L \geq 0\)

### Step 3: Initial Tableau (After Substitution)

```
BV | S  | M' | L  | s1 | s2 | s3 | RHS
---+----+----+----+----+----+----+----
s1 | 2  | 3  | 5  | 1  | 0  | 0  | 120
s2 | 4  | 2  | 5  | 0  | 1  | 0  | 160
s3 | 3  | 2  | 4  | 0  | 0  | 1  | 140
---+----+----+----+----+----+----+----
-Z | -6 | -8 | -10| 0  | 0  | 0  | -80
```

### Step 4: Iteration 1

**Entering variable:** L (most negative coefficient in -Z row: -10)
**Leaving variable:** 
- s1: 120/5 = 24
- s2: 160/5 = 32
- s3: 140/4 = 35

Minimum ratio = 24, so s1 leaves.

**Pivot:** Row 1, Column L (element = 5)

After pivot:
- New pivot row: Divide row 1 by 5
- Update other rows

```
BV | S  | M' | L  | s1 | s2 | s3 | RHS
---+----+----+----+----+----+----+----
L  | 0.4| 0.6| 1  | 0.2| 0  | 0  | 24
s2 | 2  | -1 | 0  | -1 | 1  | 0  | 40
s3 | 1.4| -0.4| 0 | -0.8| 0 | 1  | 44
---+----+----+----+----+----+----+----
-Z | -2 | -2 | 0  | 2  | 0  | 0  | 160
```

### Step 5: Iteration 2

**Entering variable:** S or M' (both have -2, choose S)
**Leaving variable:**
- L: 24/0.4 = 60
- s2: 40/2 = 20
- s3: 44/1.4 = 31.43

Minimum ratio = 20, so s2 leaves.

**Pivot:** Row 2, Column S (element = 2)

After pivot:
```
BV | S  | M' | L  | s1 | s2 | s3 | RHS
---+----+----+----+----+----+----+----
L  | 0  | 0.8| 1  | 0.4| -0.2| 0 | 16
S  | 1  | -0.5| 0 | -0.5| 0.5| 0 | 20
s3 | 0  | 0.3| 0 | -0.1| -0.7| 1 | 16
---+----+----+----+----+----+----+----
-Z | 0  | -3 | 0  | 1  | 1  | 0  | 200
```

### Step 6: Iteration 3

**Entering variable:** M' (coefficient -3)
**Leaving variable:**
- L: 16/0.8 = 20
- S: 20/(-0.5) = -40 (negative, skip)
- s3: 16/0.3 = 53.33

Minimum ratio = 20, so L leaves.

**Pivot:** Row 1, Column M' (element = 0.8)

After pivot:
```
BV | S  | M' | L  | s1 | s2 | s3 | RHS
---+----+----+----+----+----+----+----
M' | 0  | 1  | 1.25| 0.5| -0.25| 0 | 20
S  | 1  | 0  | 0.625| -0.25| 0.375| 0 | 30
s3 | 0  | 0  | -0.375| -0.25| -0.625| 1 | 10
---+----+----+----+----+----+----+----
-Z | 0  | 0  | 3.75| 2.5| 0.25| 0 | 260
```

All coefficients in -Z row are non-negative. **Optimal solution found!**

### Final Solution

- \(S = 30\)
- \(M' = 20\), so \(M = M' + 10 = 30\)
- \(L = 0\)
- \(Z = 260 + 80 = 340\) (adding back the constant from substitution)

**Optimal Value:** \(Z^* = 340\)
**Optimal Solution:** \(S^* = 30, M^* = 30, L^* = 0\)


