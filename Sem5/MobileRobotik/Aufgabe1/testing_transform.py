from Aufgabe1 import transform as t
import numpy as np

__author__ = "Josef Müller, Isabella Schön"


# ======================================
# Aufgabe 2.1
# ======================================

# a)

# T(A,B)

tab1 = np.array([-2, 0, 0])
trans_ab = t.trans(tab1)
rot_ab1 = t.rotz(180)
rot_trans_ab1 = t.rot2trans(rot_ab1)

TAB1 = np.matmul(trans_ab, rot_trans_ab1)

# T(B,C)

tbc1 = np.array([-4, -1, 0])
trans_bc1 = t.trans(tbc1)
rot_bc1 = t.rotz(270)
rot_trans_bc1 = t.rot2trans(rot_bc1)

TBC1 = np.matmul(trans_bc1, rot_trans_bc1)

# T(B,C)

tac1 = np.array([2, 1, 0])
trans_ac1 = t.trans(tac1)
rot_ac1 = t.rotz(90)
rot_trans_ac1 = t.rot2trans(rot_ac1)

TAC1 = np.matmul(trans_ac1, rot_trans_ac1)

# ======================================

# b)

TAC_check1 = np.matmul(TAB1, TBC1)
# using here allclose because it uses tolerance
# 0 is because of rounding errors not exactly 0
check_b = np.allclose(TAC_check1, TAC1)

# ======================================

# c)

tca1 = np.array([-1, 2, 0])
trans_ca1 = t.trans(tca1)
rot_ca1 = t.rotz(270)
rot_trans_ca1 = t.rot2trans(rot_ca1)

TCA1 = np.matmul(trans_ca1, rot_trans_ca1)

check_c = np.matmul(TAC1, TCA1)

# ======================================

# d)

# pA = TAB * pB
pB1 = np.array([-3, 1, 0, 1])
pA1 = np.matmul(TAB1, pB1)

# ======================================
# Aufgabe 2.2
# ======================================

# a)

# T(O,A)

oa2 = np.array([1, 1])
trans_oa2 = t.trans(oa2)
rot_oa2 = t.rot(0)
rot_trans_oa2 = t.rot2trans(rot_oa2)

TOA2 = np.matmul(trans_oa2, rot_trans_oa2)

# T(O,B)

ob2 = np.array([3, 2])
trans_ob2 = t.trans(ob2)
rot_ob2 = t.rot(30)
rot_trans_ob2 = t.rot2trans(rot_ob2)

TOB2 = np.matmul(trans_ob2, rot_trans_ob2)

# ======================================

# b)

# pO = TOB * pB

pB2 = np.array([1, 1, 1])
pO2 = np.matmul(TOB2, pB2)

# ======================================

# c)

# TAB = TAO * TOB
# TAO = (TOA)^-1

TAO2 = np.linalg.inv(TOA2)
TAB2 = np.matmul(TAO2, TOB2)

# ======================================

# d)

# pA = TAB * pB

pA2 = np.matmul(TAB2, pB2)

# ======================================

# e)

# qA = TAB * pA

qA2 = np.matmul(TAB2, pA2)
